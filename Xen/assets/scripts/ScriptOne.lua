
function OnStart()
	LogWarning("OnStart")
end

function OnUpdate(timestep)
	local transform = GetCurrentTransform()

	-- if (IsKeyPressed('left')) then
	-- 	ApplyForce2D(transform.position.x, transform.position.y, -20.0, 0.0)
	-- 	-- ApplyForceToCentre2D(-20.0, 0.0)
	-- end
	-- if (IsKeyPressed('right')) then
	-- 	ApplyForce2D(transform.position.x, transform.position.y, 20.0, 0.0)
	-- 	-- ApplyForceToCentre2D(20.0, 0.0)
	-- end	
	-- if (IsKeyPressed('up')) then
	-- 	ApplyForce2D(transform.position.x, transform.position.y, 0.0, 20.0)
	-- 	-- ApplyForceToCentre2D(0.0, 20.0)
	-- end
	-- if (IsKeyPressed('down')) then
	-- 	ApplyForce2D(transform.position.x, transform.position.y, 0.0, -20.0)
	-- 	-- ApplyForceToCentre2D(0.0, -20.0)
	-- end
	-- 
	-- if(IsMouseButtonPressed(0)) then
	-- 	LogWarning("Left: ")
	-- end

	SetLinearVelocity2D(10, 10)

	SetCurrentTransform(transform)
end
