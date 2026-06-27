premake.modules.lua = {}
local m = premake.modules.lua
local p = premake

local filesToClean = {
    -- Visual Studio
    "*.sln",
    "*.slnx",
    "*.vcxproj",
    "*.vcxproj.filters",
    "*.vcxproj.user",

    -- GNU Make
    "*.make",
    "Makefile",
    
    -- Ninja
    "*.ninja",
    "build.ninja",
    ".ninja_deps",
    ".ninja_log",
    
    -- Misc
    "compile_commands.json",
    ".vscode/tasks.json",
    ".vscode/launch.json",

    -- kill off the very executable that invoked the clean action(lol).
    "premake5.exe"
}

local buildDirectories = {
    "bin",

    -- premake stuff
    "deps/premake/bin",
    "deps/premake/build",
}

local projectDirectories = {}

newaction {
    trigger = "clean",
    shortname = "Clean",
    description = "Clean Premake generated build files and binaries",

    onProject = function(prj)
        local scriptDir = path.getdirectory(prj.script)

        if not projectDirectories[scriptDir] then
            projectDirectories[scriptDir] = true
        end

        -- Also add the current directory as a project directory
        projectDirectories["."] = true
    end,

    execute = function()
        local function removeFile(file)
            if os.isfile(file) then
                p.w("Removing file: " .. file)
                os.remove(file)
            end
        end

        local function removeDirectory(dir)
            if os.isdir(dir) then
                p.w("Removing directory: " .. dir)
                os.rmdir(dir)
            end
        end

        -- Remove generated project files.
        for dir in pairs(projectDirectories) do
            for _, pattern in ipairs(filesToClean) do
                local fullPattern = path.join(dir, pattern)

                for _, file in ipairs(os.matchfiles(fullPattern)) do
                    removeFile(file)
                end
            end
        end

        -- Remove build artifacts.
        for _, dir in ipairs(buildDirectories) do
            removeDirectory(dir)
        end

        print("Clean completed successfully.")
    end
}