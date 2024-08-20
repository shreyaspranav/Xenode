
function OnStart()
	LogWarning("OnStart")
end

function OnUpdate(timestep)
	local transform = GetCurrentTransform()

	if (IsKeyPressed('left')) then
		LogWarning("Left")
	end
	if (IsKeyPressed('right')) then
		LogWarning("Right")
	end	
	if (IsKeyPressed('up')) then
		LogWarning("Up")
	end
	if (IsKeyPressed('down')) then
		LogWarning("Down")
	end
end

function OnFixedUpdate()
	local transform = GetCurrentTransform()

	if (IsKeyPressed('left')) then
		ApplyForce2D(transform.position.x, transform.position.y, -20.0, 0.0)
	end
	if (IsKeyPressed('right')) then
		ApplyForce2D(transform.position.x, transform.position.y, 20.0, 0.0)
	end	
	if (IsKeyPressed('up')) then
		ApplyForce2D(transform.position.x, transform.position.y, 0.0, 20.0)
	end
	if (IsKeyPressed('down')) then
		ApplyForce2D(transform.position.x, transform.position.y, 0.0, -20.0)
	end

	SetCurrentTransform(transform)
end

