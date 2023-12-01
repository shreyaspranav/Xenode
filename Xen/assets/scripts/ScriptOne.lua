
function OnStart()
	LogWarning("OnStart")
end

function OnUpdate(timestep)
	local transform = GetCurrentTransform()

	if (IsKeyPressed('a')) then
		transform.Position.x = transform.Position.x - 0.003 * timestep
	end
	if (IsKeyPressed('d')) then
		transform.Position.x = transform.Position.x + 0.003 * timestep
	end	
	if (IsKeyPressed('w')) then
		transform.Position.y = transform.Position.y + 0.003 * timestep
	end
	if (IsKeyPressed('s')) then
		transform.Position.y = transform.Position.y - 0.003 * timestep
	end

	SetCurrentTransform(transform)
end
