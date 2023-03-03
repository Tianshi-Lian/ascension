import os
import sys

commands = "Valid commands: "
for f in os.scandir("./scripts"):
    if f.is_file() and f.name.endswith(".py") and not f.name.startswith("_"):
        commands += os.path.splitext(f.name)[0]
        commands += ", "

print (commands.rsplit(",", 1)[0])

sys.exit(0)
