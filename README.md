# KeyTracerCPP
A key tracing application in C++

# Explanation
  An app to track the keystrokes of a user and send an email to a specific email address at regular intervals

# Configuration
- In the SendMail.h file change the To Email Address, From Email Address and 
From Email Address Password. 
- Make sure you enable less secure apps access your source email so the program can construct and send the email. 

# How To Use
- Now run the following commands after navigating in to the 
~/KeyTracerCPP/KeyTracer-libs file
```
  g++ -std=c++11 -mwindows KeyTracer.cpp -o KeyTracer
  ./KeyTracer
```
- Now, the application has started running you can verify this by checking the windows task manager and checking if "KeyTracer" application is running
- Now an email is sent to the to address with an encrypted key log.
- To decrypt this key log you can use the TraceDecoder in 
~/KeyTracerCPP/TraceDecoder-files

```
  g++ TraceDecoder.cpp -o TraceDecoder
  ./TraceDecoder input.txt output.txt
```
