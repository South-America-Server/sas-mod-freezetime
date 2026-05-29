// -- SAS HUD - By Mindisgurpee
// -- UI Layout Figma - by Matheus Kiev

class SAS_HUD : SCR_InfoDisplayExtended
{
	protected bool isTimeFriendlyStarter = false;
	protected bool isFinishedFriendlyTime = false;

	protected SizeLayoutWidget m_wNotificationStart;
	protected RichTextWidget m_wTimeLeft;
	protected RichTextWidget m_wPlayerCount;
	protected RichTextWidget m_wTimeLeftEnd;
	protected RichTextWidget m_wPlayerCountEnd;
	protected SizeLayoutWidget m_wNotificationEnd;
	protected ImageWidget m_wProgressBar;
	protected SCR_FadeUIComponent m_FadeComponentStart;
	protected SCR_FadeUIComponent m_FadeComponentEnd;
	protected bool         m_bNotifVisible = false;
	protected Widget localRoot;
	
	// Opções locais (fallback)
	protected float	m_fNotifDuration	= 4.0;
	protected float m_fNotifOpacityShown = 0.75;
	
	protected int m_iFreezeTime = 300;
	protected string m_sSound = SCR_SoundEvent.SOUND_SIREN;
	
	// Setters
	protected bool m_bInitDone = false;
	
	protected bool m_bEnabled = true;
	
	//------------------------------------------------------------------------------------------------
	override bool DisplayStartDrawInit(IEntity owner)
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void DisplayStartDraw(IEntity owner)
	{
		if (!m_bEnabled)
			return;
		
		if (m_bInitDone)
			return;
		
		m_bInitDone = true;		
		
		SAS_FreezetimeConfig config = SAS_FreezetimeConfig.Get();
		if (config)
		{
			m_fNotifDuration = config.m_fNotifDuration;
			m_fNotifOpacityShown = config.m_fNotifOpacityShown;
			m_iFreezeTime = config.m_iFreezeTime;
			m_sSound = config.m_sSound;
		}
		
		if(!m_wRoot){
			SAS_Logs.Error("[SAS_HUD]", "Fatal Error, w_root is not set");
			return;
		}
		
		m_wNotificationStart = SizeLayoutWidget.Cast(m_wRoot.FindWidget("alertFrameStart"));
		m_wNotificationEnd = SizeLayoutWidget.Cast(m_wRoot.FindWidget("alertFrameEnd"));
		
		HideStartNotificationHard();
		HideNotificationHard();
		
		m_wTimeLeft = RichTextWidget.Cast(m_wNotificationStart.FindAnyWidget("m_wRemTime"));
		m_wPlayerCount = RichTextWidget.Cast(m_wNotificationStart.FindAnyWidget("m_wPlayerCount"));
		m_wProgressBar = ImageWidget.Cast(m_wNotificationStart.FindAnyWidget("progressBar"));
		
		m_wTimeLeftEnd = RichTextWidget.Cast(m_wNotificationEnd.FindAnyWidget("m_wRemTimeEnd"));
		m_wPlayerCountEnd = RichTextWidget.Cast(m_wNotificationEnd.FindAnyWidget("m_wPlayerCountEnd"));
		
		if(!m_wTimeLeft && !m_wPlayerCount)
		{
			SAS_Logs.Error("[SAS_HUD]", "Timer and Player count not found exiting");
			return;	
		}
		
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return;
		
		factionManager.GetOnPlayerFactionCountChanged().Insert(UpdateFactionPlayerCount);			
		
		if(!IsShown())
			Show(true);	
					
		// Initiat calling from UI Layout Notification
		GetGame().GetCallqueue().CallLater(UpdateFreezeTimeDisplay, 1000, false);
	}


	void SendNotification(int timer = 5000)
	{
		if (!m_wNotificationEnd)
		{
			SAS_Logs.Error("[SAS_HUD]","Notification End Layout was not set");
			return;
		}

		ShowNotification();
		PlayNotificationSound();

		int displayMs = timer;
		if (displayMs < m_iFreezeTime+10)  displayMs = m_iFreezeTime + 10;
		if (displayMs > 60000) displayMs = 60000;

		GetGame().GetCallqueue().Remove(HideNotification);
		GetGame().GetCallqueue().CallLater(HideNotification, displayMs, false);
	}	

	protected void ShowNotification()
	{
		if (!m_wNotificationEnd) return;

		m_wNotificationEnd.SetVisible(true);
		m_wNotificationEnd.SetOpacity(0);
		m_bNotifVisible = true;
					
		if(m_wNotificationEnd)
			m_FadeComponentEnd = SCR_FadeUIComponent.Cast(m_wNotificationEnd.FindHandler(SCR_FadeUIComponent));
		
		if(m_FadeComponentEnd)
		{
			if(!m_FadeComponentEnd.IsFading())
				m_FadeComponentEnd.FadeIn(true);
		}

	}

	protected void HideNotification()
	{
		if (!m_wNotificationEnd || !m_bNotifVisible)
			return;
		
		if (m_FadeComponentEnd)
		{
			m_FadeComponentEnd.FadeOut(false);
			m_bNotifVisible = false;
		}
		else
		{
			HideNotificationHard();
		}
	}

	protected void HideNotificationHard()
	{
		if (!m_wNotificationEnd) return;

		m_wNotificationEnd.SetVisible(false);
		m_wNotificationEnd.SetOpacity(0);
		m_bNotifVisible = false;
	}

	protected void HideStartNotificationHard()
	{
		if (!m_wNotificationStart) return;

		m_wNotificationStart.SetVisible(false);
		m_wNotificationStart.SetOpacity(0);
	}

	protected void PlayNotificationSound()
	{
		SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.TASK_CREATED, true);
	}
	
	protected void UpdateFactionPlayerCount(Faction faction, int playerCountParam)
	{
		SCR_Faction factionScripted = SCR_Faction.Cast(faction);
		
		if (!factionScripted)
			return;	
		
		int playerLimit = factionScripted.GetPlayerLimit();
		
		if (m_wPlayerCount)
		{
			if (playerLimit > 0)
				m_wPlayerCount.SetTextFormat("#AR-SupportStation_ActionFormat_ItemAmount", playerCountParam, playerLimit);
			else
				m_wPlayerCount.SetText(playerCountParam.ToString());
		}
		
		if (m_wPlayerCountEnd)
		{
			if (playerLimit > 0)
				m_wPlayerCountEnd.SetTextFormat("#AR-SupportStation_ActionFormat_ItemAmount", playerCountParam, playerLimit);
			else
				m_wPlayerCountEnd.SetText(playerCountParam.ToString());
		}
	}
	
	// - Start Tick  Initial LayoutLoad
	void UpdateFreezeTimeDisplay()
	{		
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!gameMode)
		{
			GetGame().GetCallqueue().CallLater(UpdateFreezeTimeDisplay, 1000, false);
			return;
		}
		
		float gameRunTime = gameMode.GetElapsedTime();

		if (!isTimeFriendlyStarter && gameRunTime > m_iFreezeTime)
		{
			HideStartNotificationHard();
			HideNotificationHard();
			return;		
		}
		
		if (!m_wNotificationStart && !m_wNotificationEnd) 
		{
			SAS_Logs.Error("[SAS_HUD]", "Fatal Error, main notification layout not set");
			return;
		}
		
		if(!isFinishedFriendlyTime) 
			m_FadeComponentStart = SCR_FadeUIComponent.Cast(m_wNotificationStart.FindHandler(SCR_FadeUIComponent));
				
		if (!isTimeFriendlyStarter)
		{							
			m_wNotificationStart.SetVisible(true);
			m_wNotificationStart.SetOpacity(0);				
		
			if(m_FadeComponentStart)
				m_FadeComponentStart.FadeIn(true);	
								
			SCR_UISoundEntity.SoundEvent(m_sSound);
			isTimeFriendlyStarter = true;			
		}

		if (gameRunTime < m_iFreezeTime)
		{	
			float currentTime = Math.Floor(m_iFreezeTime - gameRunTime);
			if (m_wTimeLeft)
				m_wTimeLeft.SetText(SCR_FormatHelper.FormatTime(currentTime));					
			if (m_wTimeLeftEnd)
				m_wTimeLeftEnd.SetText(SCR_FormatHelper.FormatTime(currentTime));
			if (m_wProgressBar)
				m_wProgressBar.SetSize(calcProgressBar(currentTime), 5);			
			GetGame().GetCallqueue().CallLater(UpdateFreezeTimeDisplay, 1000, false);
		}
		else
		{			
			if (!isFinishedFriendlyTime)
			{
				if(m_FadeComponentStart)
					m_FadeComponentStart.FadeOut(true);
				else
					HideStartNotificationHard();
											
				isFinishedFriendlyTime = true;				
				SendNotification();
				GetGame().GetCallqueue().CallLater(UpdateFreezeTimeDisplay, 3000, false);	
			}
			else
			{
				if (m_FadeComponentEnd) 
					m_FadeComponentEnd.FadeOut(true);
				else
					HideNotificationHard();
			}
		}
	}
	
	// calc progress bar
	float calcProgressBar(float currentTime)
	{
			
		float totalImageSize = 550;
		float imageSize = (currentTime / m_iFreezeTime) * totalImageSize;
		
		if (imageSize > 0 && imageSize < totalImageSize)
			return totalImageSize - imageSize;
		
		return 550;	
	}
}

