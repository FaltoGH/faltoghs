# Source - https://stackoverflow.com/a/70589204
# Posted by Karthik Kallur
# Retrieved 2026-06-17, License - CC BY-SA 4.0

import subprocess
import os
import sys

def f(target:str="User"):
    print("\r\n\r\n-----target: %s-----"%target)
    completedProcess = subprocess.run(
        ["powershell",
        "-Command",
        "[Environment]::GetEnvironmentVariable('Path','%s')"%target],
        capture_output=True
    )

    decodedStderr =completedProcess.stderr.decode("euc-kr")
    print(decodedStderr)

    decodedStdout=completedProcess.stdout.decode("ascii")

    user_path = decodedStdout.split(";")

    for up in user_path:
        up = up.strip()
        if up:
            print("%-70s %s"%(up,os.path.exists(up)))

if __name__ == "__main__":
    if sys.platform != "win32":
        raise OSError("Only win32 platform is supported.")
    f()
    f("Machine")

