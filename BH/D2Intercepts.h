#pragma once

VOID GameDraw_Interception();
void GameAutomapDraw_Interception();
VOID OOGDraw_Interception();

void GameLoop_Interception();


VOID GamePacketRecv_Interception();
VOID ChatPacketRecv_Interception();
VOID RealmPacketRecv_Interception();
void GameInput_Interception(void);
void ChannelInput_Interception(void);
void ChannelWhisper_Interception(void);
void ChannelChat_Interception(void);
void ChannelEmote_Interception(void);
void PositionMenuTab_Interception(void);

// Tab rework
void DisplayExpansionUI_Interception(void);
void DisplayExpansionUILeftTabTopRight_Interception(void);
void DisplayExpansionUILeftTabMiddleTop_Interception(void);
void DisplayExpansionUILeftTabBottomLeft_Interception(void);
void DisplayExpansionUILeftTabBottomRight_Interception(void);
