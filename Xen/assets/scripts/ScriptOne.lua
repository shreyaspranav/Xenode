
function OnStart()
	LogWarning("OnStart")
end

function OnUpdate(timestep)
	local transform = GetCurrentTransform()

	if (IsKeyPressed('left')) then
		ApplyForceToCentre2D(-20.0, 0.0)
	end
	if (IsKeyPressed('right')) then
		ApplyForceToCentre2D(20.0, 0.0)
	end	
	if (IsKeyPressed('up')) then
		ApplyForceToCentre2D(0.0, 20.0)
	end
	if (IsKeyPressed('down')) then
		ApplyForceToCentre2D(0.0, -20.0)
	end

	SetCurrentTransform(transform)
end
