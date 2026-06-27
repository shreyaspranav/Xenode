premake.modules.lua = {}
local m = premake.modules.lua
local p = premake

local allConfigs = {}

local startProjectName = ""
local startProjectScriptDir = ""
local startProjectConfigs = {}

local configInputsTemplate = [[
"inputs": [
    {
      "id": "buildConfig",
      "type": "pickString",
      "description": "Select Premake configuration",
      "options": [{OPTIONS}]
    }
  ]
]]

local tasksJsonTemplate = [[
{
  "version": "2.0.0",
  {CONFIG_INPUTS},
  "tasks": [
    {
      "label": "Set Configuration",
      "type": "shell",
      "command": "{SET_CONFIG_COMMAND}",
      "group": {
        "kind": "build",
        "isDefault": false
      }
    },
    {
      "label": "Build",
      "type": "shell",
      "command": "{BUILD_COMMAND}",
      "group": {
        "kind": "build",
        "isDefault": true
      },
      "dependsOn": "Set Configuration"
    }
  ]
}
]]

local launchConfigJsonTemplate = [[
    {
      "name": "{CONFIG_NAME}",
      "type": "cppvsdbg",
      "request": "launch",
      "program": "{LAUNCH_EXECUTABLE}",
      "cwd": "{LAUNCH_EXECUTABLE_WD}",
    }
]]

local launchJsonTemplate = [[
{
  "version": "0.2.0",
  "configurations": [
    {LAUNCH_CONFIGS}
  ]
}
]]

local function render(template, vars)
    return (template:gsub("{([%w_]+)}", function(key)
        return vars[key] or ""
    end))
end

newaction {
    trigger = "vscode",
    shortname = "Visual Studio Code",
    description = "Generate VSCode tasks.json and launch.json",

    onWorkspace = function(wks)
        startProjectName = wks.startproject
    end,
    
	onProject = function(prj)
        -- Collect all the configurations used by the project (Should be Debug, Release_Debug and Production)
        for _, projectConfig in ipairs(prj.configurations) do
            if not allConfigs[projectConfig] then
                allConfigs[projectConfig] = true
            end
        end

        -- Get the path of the start project's executable. This will be used in launch.json 
        if startProjectName == prj.name then
            startProjectScriptDir = prj.script
            for cfg in p.project.eachconfig(prj) do
                startProjectConfigs[cfg.name] = cfg.buildtarget.abspath
            end
        end
    end,

    execute = function()
        -- Write tasks.json and launch.json
        -- List of tasks: select config, build project
        local vscodeDir = ".vscode"
        os.mkdir(vscodeDir)

        -- Compute all the values, substitute them, open the file. write the file and then close the file.

        -- tasks.json: ----------------------------------------------------
        local configOptions = {}

        for cfg, _ in pairs(allConfigs) do
            table.insert(configOptions, string.format('"%s"', cfg))
        end

        table.sort(configOptions)

        configOptions = table.concat(configOptions, ", ")

        local buildCommand = ""
        local setConfigCommand = ""

        if _TARGET_OS == "windows" then
            buildCommand = [[scripts\\Build.bat ${input:buildConfig}]]
            setConfigCommand = [[scripts\\SetConfig.bat ${input:buildConfig}]]
        elseif _TARGET_OS == "linux" then
            buildCommand = "make config=${input:buildConfig}"
            setConfigCommand = [[Not Implemented :(]]
        else
            p.error("Invalid target OS: " .. _TARGET_OS)
        end

        local configInputsJson = render(configInputsTemplate, {
            OPTIONS = configOptions
        })

        local tasksJson = render(tasksJsonTemplate, {
            CONFIG_INPUTS = configInputsJson,
            SET_CONFIG_COMMAND = setConfigCommand,
            BUILD_COMMAND = buildCommand
        })
        --------------------------------------------------------------------
        -- launch.json: ----------------------------------------------------
        
        -- Generate one launch config for each config
        -- local launchConfigJson = ""
        -- for configName, executablePath in pairs(startProjectConfigs) do
        --     local launchConfig = render(launchConfigJsonTemplate, {
        --         CONFIG_NAME = configName,
        --         LAUNCH_EXECUTABLE = executablePath
        --     })

        --     launchConfigJson = launchConfigJson .. launchConfig .. ",\n"
        -- end

        -- Generate one lauch config for the config specified in --config argument
        local launchConfigJson = render(launchConfigJsonTemplate, {
            CONFIG_NAME = "Debug " .. startProjectName,
            LAUNCH_EXECUTABLE = startProjectConfigs[_OPTIONS["config"]],
            LAUNCH_EXECUTABLE_WD = path.getdirectory(startProjectScriptDir)
        })
        

        local launchJson = render(launchJsonTemplate, {
            LAUNCH_CONFIGS = launchConfigJson
        })
        -- -----------------------------------------------------------------
        local tasks = io.open(path.join(vscodeDir, "tasks.json"), "w")
        tasks:write(tasksJson)
        tasks:close()

        local launch = io.open(path.join(vscodeDir, "launch.json"), "w")
        launch:write(launchJson)
        launch:close()
    end
}

newoption {
    trigger = "config",
    description = "Set the launch configuration for launch.json (ONLY FOR VSCODE)",
    allowed = {
        { "Debug" },
        { "Release_Debug" },
        { "Production" }
    },
    default = "Debug"
}