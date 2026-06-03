modded class SCR_BaseGameMode : BaseGameMode
{
	[RplProp()]
	protected int m_iSASFreezeTime;

	[RplProp()]
	protected float m_fSASNotifDuration;

	[RplProp()]
	protected float m_fSASNotifOpacityShown;

	[RplProp()]
	protected string m_sSASSound;

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (Replication.IsServer())
		{
			SAS_FreezetimeConfig config = SAS_FreezetimeConfig.Get();
			m_iSASFreezeTime = config.m_iFreezeTime;
			m_fSASNotifDuration = config.m_fNotifDuration;
			m_fSASNotifOpacityShown = config.m_fNotifOpacityShown;
			m_sSASSound = config.m_sSound;
			Replication.BumpMe();
		}
	}

	int GetSASFreezeTime() { return m_iSASFreezeTime; }
	float GetSASNotifDuration() { return m_fSASNotifDuration; }
	float GetSASNotifOpacityShown() { return m_fSASNotifOpacityShown; }
	string GetSASSound() { return m_sSASSound; }
}
