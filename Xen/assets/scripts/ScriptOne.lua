
function OnStart()
	LogWarning("OnStart")
end

function OnUpdate(timestep)
	local transform = GetCurrentTransform()

	if (IsKeyPressed('a')) then
		ApplyForceToCentre2D(-20.0, 0.0)
	end
	if (IsKeyPressed('d')) then
		ApplyForceToCentre2D(20.0, 0.0)
	end	
	if (IsKeyPressed('w')) then
		ApplyForceToCentre2D(0.0, 20.0)
	end
	if (IsKeyPressed('s')) then
		ApplyForceToCentre2D(0.0, -20.0)
	end

	SetCurrentTransform(transform)
end
