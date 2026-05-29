class SAS_FreezetimeConfig
{
	float m_fNotifDuration = 4.0;
	float m_fNotifOpacityShown = 0.75;
	int m_iFreezeTime = 300;
	string m_sSound = "SOUND_SIREN"; // Default sound

	protected static ref SAS_FreezetimeConfig s_Instance;

	//------------------------------------------------------------------------------------------------
	static SAS_FreezetimeConfig Get()
	{
		if (!s_Instance)
		{
			s_Instance = new SAS_FreezetimeConfig();
			s_Instance.Load();
		}
		return s_Instance;
	}

	//------------------------------------------------------------------------------------------------
	void Load()
	{
		string filePath = "$profile:FreezetimerConfig.json";
		
		// If config file doesn't exist, create it with default values
		if (!FileIO.FileExists(filePath))
		{
			JsonSaveContext saveCtx = new JsonSaveContext();
			saveCtx.WriteValue("notificationDuration", m_fNotifDuration);
			saveCtx.WriteValue("notificationOpacityShow", m_fNotifOpacityShown);
			saveCtx.WriteValue("freezeTimer", m_iFreezeTime);
			saveCtx.WriteValue("freezetimeStartSound", m_sSound);
			saveCtx.SaveToFile(filePath);
			return;
		}

		// Read and load context from JSON
		JsonLoadContext loadCtx = new JsonLoadContext();
		if (loadCtx.LoadFromFile(filePath))
		{
			loadCtx.ReadValue("notificationDuration", m_fNotifDuration);
			loadCtx.ReadValue("notificationOpacityShow", m_fNotifOpacityShown);
			loadCtx.ReadValue("freezeTimer", m_iFreezeTime);
			loadCtx.ReadValue("freezetimeStartSound", m_sSound);
		}
	}
}
