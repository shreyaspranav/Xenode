
function OnStart()
	LogWarning("OnStart")
end

function OnUpdate(timestep)
	transform = GetCurrentTransform()
	transform.Position.x = transform.Position.x + 0.003 * timestep

	SetCurrentTransform(transform)
end
