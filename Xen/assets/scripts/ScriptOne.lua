
function OnStart()
	LogWarning("OnStart")
end

function OnUpdate(timestep)
	transform = GetCurrentTransform()
	LogWarning("OnUpdate, timestep: " .. timestep)
	LogInfo("Current Position: X:" .. transform.Position.x .. " Y:" .. transform.Position.y .. " Z:" .. transform.Position.z)
end
