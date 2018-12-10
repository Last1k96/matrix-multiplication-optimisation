path = "bin\\x64\\Release\\"
import glob, os, subprocess

for fn in glob.glob(path + "*.exe"):
    p = subprocess.Popen([fn])
    p.wait()