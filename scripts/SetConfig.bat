@echo off
setlocal

: Sets the context: generates compile_commands.json and launch.json for a particular config.
premake5 vscode --config=%~1

: Setting the compiler as msc-v143 (vs2022) as there is no practival difference between the msc-v145 one
premake5 compilecommands --cc-config="%~1" --cc=msc-v143

endlocal