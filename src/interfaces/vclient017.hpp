#pragma once

#include <cstdint>
#include <sys/cdefs.h>

#include "../classes/createinterface.hpp"
#include "../classes/datatable.hpp"
#include "../classes/vector3.hpp"
#include "../classes/defs.hpp"

class CGlobalVarsBase;
class ScreenFade_t;
struct vrect_t;
class bf_read;
class bf_write;
class CEngineSprite;
class CSaveRestoreData;
class datamap_t;
class typedescription_t;
class CStandardRecvProxies;
class IFileList;
class CMouthInfo;
class CRenamedRecvTableInfo;
class IConVar;

struct ButtonCode_t;
class CViewSetup;
struct qboolean;
class Vector;
class color32;
class INetChannelInfo;
class IAchievementMgr;

#define MAX_PLAYER_NAME_LENGTH 32
#define SIGNED_GUID_LEN 32
#define MAX_CUSTOM_FILES 4

class IBaseClientDLL
{
      public:
	virtual int Init(CreateInterfaceFn appSystemFactory, CreateInterfaceFn physicsFactory,
			 CGlobalVarsBase *pGlobals)								 = 0;
	virtual void PostInit()											 = 0;
	virtual void Shutdown(void)										 = 0;
	virtual bool ReplayInit(CreateInterfaceFn replayFactory)						 = 0;
	virtual bool ReplayPostInit()										 = 0;
	virtual void LevelInitPreEntity(char const *pMapName)							 = 0;
	virtual void LevelInitPostEntity()									 = 0;
	virtual void LevelShutdown(void)									 = 0;
	virtual ClientClass *GetAllClasses(void)								 = 0;
	virtual int HudVidInit(void)										 = 0;
	virtual void HudProcessInput(bool bActive)								 = 0;
	virtual void HudUpdate(bool bActive)									 = 0;
	virtual void HudReset(void)										 = 0;
	virtual void HudText(const char *message)								 = 0;
	virtual void IN_ActivateMouse(void)									 = 0;
	virtual void IN_DeactivateMouse(void)									 = 0;
	virtual void IN_Accumulate(void)									 = 0;
	virtual void IN_ClearStates(void)									 = 0;
	virtual bool IN_IsKeyDown(const char *name, bool &isdown)						 = 0;
	virtual void IN_OnMouseWheeled(int nDelta)								 = 0;
	virtual int IN_KeyEvent(int eventcode, ButtonCode_t keynum, const char *pszCurrentBinding)		 = 0;
	virtual void CreateMove(int sequence_number, float input_sample_frametime, bool active)			 = 0;
	virtual void ExtraMouseSample(float frametime, bool active)						 = 0;
	virtual bool WriteUsercmdDeltaToBuffer(bf_write *buf, int from, int to, bool isnewcommand)		 = 0;
	virtual void EncodeUserCmdToBuffer(bf_write &buf, int slot)						 = 0;
	virtual void DecodeUserCmdFromBuffer(bf_read &buf, int slot)						 = 0;
	virtual void View_Render(vrect_t *rect)									 = 0;
	virtual void RenderView(const CViewSetup &view, int nClearFlags, int whatToDraw)			 = 0;
	virtual void View_Fade(ScreenFade_t *pSF)								 = 0;
	virtual void SetCrosshairAngle(const QAngle &angle)							 = 0;
	virtual void InitSprite(CEngineSprite *pSprite, const char *loadname)					 = 0;
	virtual void ShutdownSprite(CEngineSprite *pSprite)							 = 0;
	virtual int GetSpriteSize(void) const									 = 0;
	virtual void VoiceStatus(int entindex, qboolean bTalking)						 = 0;
	virtual void InstallStringTableCallback(char const *tableName)						 = 0;
	virtual void FrameStageNotify(ClientFrameStage_t curStage)						 = 0;
	virtual bool DispatchUserMessage(int msg_type, bf_read &msg_data)					 = 0;
	virtual CSaveRestoreData *SaveInit(int size)								 = 0;
	virtual void SaveWriteFields(CSaveRestoreData *, const char *, void *, datamap_t *, typedescription_t *,
				     int)									 = 0;
	virtual void SaveReadFields(CSaveRestoreData *, const char *, void *, datamap_t *, typedescription_t *,
				    int)									 = 0;
	virtual void PreSave(CSaveRestoreData *)								 = 0;
	virtual void Save(CSaveRestoreData *)									 = 0;
	virtual void WriteSaveHeaders(CSaveRestoreData *)							 = 0;
	virtual void ReadRestoreHeaders(CSaveRestoreData *)							 = 0;
	virtual void Restore(CSaveRestoreData *, bool)								 = 0;
	virtual void DispatchOnRestore()									 = 0;
	virtual CStandardRecvProxies *GetStandardRecvProxies()							 = 0;
	virtual void WriteSaveGameScreenshot(const char *pFilename)						 = 0;
	virtual void EmitSentenceCloseCaption(char const *tokenstream)						 = 0;
	virtual void EmitCloseCaption(char const *captionname, float duration)					 = 0;
	virtual bool CanRecordDemo(char *errorMsg, int length) const						 = 0;
	virtual void OnDemoRecordStart(char const *pDemoBaseName)						 = 0;
	virtual void OnDemoRecordStop()										 = 0;
	virtual void OnDemoPlaybackStart(char const *pDemoBaseName)						 = 0;
	virtual void OnDemoPlaybackStop()									 = 0;
	virtual bool ShouldDrawDropdownConsole()								 = 0;
	virtual int GetScreenWidth()										 = 0;
	virtual int GetScreenHeight()										 = 0;
	virtual void WriteSaveGameScreenshotOfSize(const char *pFilename, int width, int height,
						   bool bCreatePowerOf2Padded = false, bool bWriteVTF = false)	 = 0;
	virtual bool GetPlayerView(CViewSetup &playerView)							 = 0;
	virtual void SetupGameProperties(/*CUtlVector< XUSER_CONTEXT>*/ void *&contexts,
					 /*CUtlVector< XUSER_PROPERTY >*/ void *&properties) = 0;
	virtual uint32_t GetPresenceID(const char *pIDName)								 = 0;
	virtual const char *GetPropertyIdString(const uint32_t id)							 = 0;
	virtual void GetPropertyDisplayString(uint32_t id, uint32_t value, char *pOutput, int nBytes)			 = 0;
	virtual void InvalidateMdlCache()									 = 0;
	virtual void IN_SetSampleTime(float frametime)								 = 0;
	virtual void ReloadFilesInList(IFileList *pFilesToReload)						 = 0;
	virtual bool HandleUiToggle()										 = 0;
	virtual bool ShouldAllowConsole()									 = 0;
	virtual CRenamedRecvTableInfo *GetRenamedRecvTableInfos()						 = 0;
	virtual CMouthInfo *GetClientUIMouthInfo()								 = 0;
	virtual void FileReceived(const char *fileName, unsigned int transferID)				 = 0;
	virtual const char *TranslateEffectForVisionFilter(const char *pchEffectType, const char *pchEffectName) = 0;
	virtual void ClientAdjustStartSoundParams(struct StartSoundParams_t &params)				 = 0;
	virtual bool DisconnectAttempt(void)									 = 0;
	virtual bool IsConnectedUserInfoChangeAllowed(IConVar *pCvar)						 = 0;
};

class CHLClient : public IBaseClientDLL
{
      public:
	CHLClient();

	virtual int Init(CreateInterfaceFn appSystemFactory, CreateInterfaceFn physicsFactory,
			 CGlobalVarsBase *pGlobals);

	virtual void PostInit();
	virtual void Shutdown(void);

	virtual bool ReplayInit(CreateInterfaceFn fnReplayFactory);
	virtual bool ReplayPostInit();

	virtual void LevelInitPreEntity(const char *pMapName);
	virtual void LevelInitPostEntity();
	virtual void LevelShutdown(void);

	virtual ClientClass *GetAllClasses(void);

	virtual int HudVidInit(void);
	virtual void HudProcessInput(bool bActive);
	virtual void HudUpdate(bool bActive);
	virtual void HudReset(void);
	virtual void HudText(const char *message);

	virtual void IN_ActivateMouse(void);
	virtual void IN_DeactivateMouse(void);
	virtual void IN_Accumulate(void);
	virtual void IN_ClearStates(void);
	virtual bool IN_IsKeyDown(const char *name, bool &isdown);
	virtual void IN_OnMouseWheeled(int nDelta);

	virtual int IN_KeyEvent(int eventcode, ButtonCode_t keynum, const char *pszCurrentBinding);
	virtual void IN_SetSampleTime(float frametime);

	virtual void CreateMove(int sequence_number, float input_sample_frametime, bool active);
	virtual void ExtraMouseSample(float frametime, bool active);
	virtual bool WriteUsercmdDeltaToBuffer(bf_write *buf, int from, int to, bool isnewcommand);
	virtual void EncodeUserCmdToBuffer(bf_write &buf, int slot);
	virtual void DecodeUserCmdFromBuffer(bf_read &buf, int slot);

	virtual void View_Render(vrect_t *rect);
	virtual void RenderView(const CViewSetup &view, int nClearFlags, int whatToDraw);
	virtual void View_Fade(ScreenFade_t *pSF);

	virtual void SetCrosshairAngle(const QAngle &angle);

	virtual void InitSprite(CEngineSprite *pSprite, const char *loadname);
	virtual void ShutdownSprite(CEngineSprite *pSprite);

	virtual int GetSpriteSize(void) const;

	virtual void VoiceStatus(int entindex, qboolean bTalking);

	virtual void InstallStringTableCallback(const char *tableName);

	virtual void FrameStageNotify(ClientFrameStage_t curStage);

	virtual bool DispatchUserMessage(int msg_type, bf_read &msg_data);

	virtual CSaveRestoreData *SaveInit(int size);
	virtual void SaveWriteFields(CSaveRestoreData *, const char *, void *, datamap_t *, typedescription_t *, int);
	virtual void SaveReadFields(CSaveRestoreData *, const char *, void *, datamap_t *, typedescription_t *, int);
	virtual void PreSave(CSaveRestoreData *);
	virtual void Save(CSaveRestoreData *);
	virtual void WriteSaveHeaders(CSaveRestoreData *);
	virtual void ReadRestoreHeaders(CSaveRestoreData *);
	virtual void Restore(CSaveRestoreData *, bool);
	virtual void DispatchOnRestore();
	virtual void WriteSaveGameScreenshot(const char *pFilename);

	virtual void EmitSentenceCloseCaption(char const *tokenstream);
	virtual void EmitCloseCaption(char const *captionname, float duration);

	virtual CStandardRecvProxies *GetStandardRecvProxies();

	virtual bool CanRecordDemo(char *errorMsg, int length) const;

	virtual void OnDemoRecordStart(char const *pDemoBaseName);
	virtual void OnDemoRecordStop();
	virtual void OnDemoPlaybackStart(char const *pDemoBaseName);
	virtual void OnDemoPlaybackStop();

	virtual bool ShouldDrawDropdownConsole();

	virtual int GetScreenWidth();
	virtual int GetScreenHeight();

	virtual void WriteSaveGameScreenshotOfSize(const char *pFilename, int width, int height,
						   bool bCreatePowerOf2Padded /*=false*/, bool bWriteVTF /*=false*/);

	virtual bool GetPlayerView(CViewSetup &playerView);

	virtual void SetupGameProperties(/*CUtlVector< XUSER_CONTEXT >*/ void *&contexts,
					 /*CUtlVector< XUSER_PROPERTY >*/ void *&properties);
	virtual uint32_t GetPresenceID(const char *pIDName);
	virtual const char *GetPropertyIdString(const uint32_t id);
	virtual void GetPropertyDisplayString(uint32_t id, uint32_t value, char *pOutput, int nBytes);
	virtual void StartStatsReporting(void *handle, bool bArbitrated);

	virtual void InvalidateMdlCache();

	virtual void ReloadFilesInList(IFileList *pFilesToReload);

	virtual bool HandleUiToggle();

	virtual bool ShouldAllowConsole();

	virtual CRenamedRecvTableInfo *GetRenamedRecvTableInfos();

	virtual CMouthInfo *GetClientUIMouthInfo();

	virtual void FileReceived(const char *fileName, unsigned int transferID);

	virtual const char *TranslateEffectForVisionFilter(const char *pchEffectType, const char *pchEffectName);

	virtual void ClientAdjustStartSoundParams(struct StartSoundParams_t &params);

	virtual bool DisconnectAttempt(void);

      public:
	//void PrecacheMaterial(const char *pMaterialName);

	virtual bool IsConnectedUserInfoChangeAllowed(IConVar *pCvar);

      private:
	//void UncacheAllMaterials();
	//void ResetStringTablePointers();

	/*CUtlVector< IMaterial * >*/ void *m_CachedMaterials;
};

CHLClient* v_client();