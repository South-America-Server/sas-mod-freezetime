// Vehicle Controllers Component
// Objective: Disable or prevent vehicle starts before freeze time ends.

modded class VehicleControllerComponent : BaseVehicleControllerComponent
{
	protected int m_iFreezeTime = 300;
	//------------------------------------------------------------------------------------------------
	override bool OnBeforeEngineStart()
	{
		if (!IsStarterFunctional())
			return false;	
		
		return isInFreezeTime();
	}
	
	bool isInFreezeTime()
	{
		float gameElapsedTime;		
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		
		if (gameMode)
			gameElapsedTime = gameMode.GetElapsedTime();
		else 
			gameElapsedTime = float.MAX;
		
		//Set total minimum time to StartVehicles
		if(gameElapsedTime < m_iFreezeTime)
			return false;
		
		return true;
	}	
	
} 