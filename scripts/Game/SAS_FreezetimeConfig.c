class SAS_FreezetimeConfig
{
	float m_fNotifDuration = 4.0;
	float m_fNotifOpacityShown = 0.75;
	int m_iFreezeTime = 300;
	string m_sSound = "SOUND_SIREN"; // Default sound

	protected bool m_isLoaded = false;

	protected const string PROFILE_CONFIG_PATH = "$profile:SAS_Configs/FreezetimerConfig.json";
	protected static ref SAS_FreezetimeConfig s_Instance;

	//------------------------------------------------------------------------------------------------
	static SAS_FreezetimeConfig Get()
	{
		if (Replication.IsServer())
		{
			if (!s_Instance)
			{
				s_Instance = new SAS_FreezetimeConfig();
				if (!s_Instance.Load(PROFILE_CONFIG_PATH))
				{
					SAS_Logs.Error("FreezetimeConfig", "Failed to load configuration file.");
				}
			}
			return s_Instance;
		}
		else
		{
			// Client side: pull from replicated GameMode
			SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
			if (gameMode)
			{
				ref SAS_FreezetimeConfig clientInstance = new SAS_FreezetimeConfig();
				clientInstance.m_fNotifDuration = gameMode.GetSASNotifDuration();
				clientInstance.m_fNotifOpacityShown = gameMode.GetSASNotifOpacityShown();
				clientInstance.m_iFreezeTime = gameMode.GetSASFreezeTime();
				clientInstance.m_sSound = gameMode.GetSASSound();
				clientInstance.m_isLoaded = true;
				return clientInstance;
			}
		}

		// Fallback for offline/early loading
		if (!s_Instance)
		{
			s_Instance = new SAS_FreezetimeConfig();
			s_Instance.Load(PROFILE_CONFIG_PATH);
		}
		return s_Instance;
	}

	//------------------------------------------------------------------------------------------------
	bool IsLoaded()
	{
		return m_isLoaded;
	}

	//------------------------------------------------------------------------------------------------
	bool Load(string filePath)
	{
		if (IsLoaded())
		{
			return true;
		}

		// If config file doesn't exist, create it with default values
		if (!FileIO.FileExists(filePath))
		{
			if (!GenerateDefaultConfigFile(filePath))
			{
				SAS_Logs.Error("FreezetimeConfig", string.Format("Configuration file not found and template creation failed: %1", filePath));
				m_isLoaded = false;
				return false;
			}
			SAS_Logs.Warn("FreezetimeConfig", string.Format("Configuration file created at %1.", filePath));
		}

		// Read and load context from JSON
		JsonLoadContext loadCtx = new JsonLoadContext();
		if (!loadCtx.LoadFromFile(filePath))
		{
			SAS_Logs.Error("FreezetimeConfig", "Failed to parse JSON configuration: " + filePath);
			m_isLoaded = false;
			return false;
		}

		loadCtx.ReadValue("notificationDuration", m_fNotifDuration);
		loadCtx.ReadValue("notificationOpacityShow", m_fNotifOpacityShown);
		loadCtx.ReadValue("freezeTimer", m_iFreezeTime);
		loadCtx.ReadValue("freezetimeStartSound", m_sSound);

		m_isLoaded = true;
		SAS_Logs.Info("FreezetimeConfig", "Configuration loaded successfully from: " + filePath);
		SAS_Logs.Info("FreezetimeConfig", string.Format("Loaded parameters - notificationDuration: %1, notificationOpacityShow: %2, freezeTimer: %3, freezetimeStartSound: '%4'", m_fNotifDuration, m_fNotifOpacityShown, m_iFreezeTime, m_sSound));
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool GenerateDefaultConfigFile(string filePath)
	{
		// Ensure profile directory exists (matches PROFILE_CONFIG_PATH folder)
		FileIO.MakeDirectory("$profile:SAS_Configs");

		JsonSaveContext saveCtx = new JsonSaveContext();
		bool success = true;
		success &= saveCtx.WriteValue("notificationDuration", m_fNotifDuration);
		success &= saveCtx.WriteValue("notificationOpacityShow", m_fNotifOpacityShown);
		success &= saveCtx.WriteValue("freezeTimer", m_iFreezeTime);
		success &= saveCtx.WriteValue("freezetimeStartSound", m_sSound);

		if (success)
		{
			success = saveCtx.SaveToFile(filePath);
		}
		return success;
	}
}
