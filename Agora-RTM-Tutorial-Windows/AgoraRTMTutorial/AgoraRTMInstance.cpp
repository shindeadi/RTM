#include "stdafx.h"
#include "AgoraRTMInstance.h"
#include "commonFun.h"

CAgoraRTMInstance* CAgoraRTMInstance::m_pAgoraRTMInstance = NULL;

std::string _generateObjectId()
{
    GUID guid;
    std::string objId = "";
    if (S_OK == CoCreateGuid(&guid)) {
        char buf[33] = { 0 };

        _snprintf_s(buf, sizeof(buf)
            , "%08X%04X%04x%02X%02X%02X%02X%02X%02X%02X%02X"
            , guid.Data1
            , guid.Data2
            , guid.Data3
            , guid.Data4[0], guid.Data4[1]
            , guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
            , guid.Data4[6], guid.Data4[7]
        );
        objId = buf;
    }
    return objId;
}



CAgoraRTMInstance* CAgoraRTMInstance::getSignalInstance(const std::string &appId, CRTMCallBack* callBack)
{
  if (NULL == m_pAgoraRTMInstance){

    m_pAgoraRTMInstance = new CAgoraRTMInstance(appId, callBack);
  }

  return m_pAgoraRTMInstance;
}

void CAgoraRTMInstance::ReleaseInstance() {
}

void CAgoraRTMInstance::InitRTMService() {
  m_rtmService = createRtmService();
  if (!m_rtmService) {
    exit(0);
  }
  int ret = m_rtmService->initialize(m_AppId.c_str(), m_RtmCallback);
  if (ret) {
    exit(0);
  }
}

CAgoraRTMInstance::CAgoraRTMInstance(const std::string appId, CRTMCallBack* callBack)
: m_ChatMsg(nullptr),
m_loginWnd(nullptr),
m_isLoginPt(false),
m_isJoinChannel(false),
m_Account(""),
m_ChannelName(""),
m_AppId(appId),
m_AppCertificateId(""),
m_RtmCallback(callBack),
m_rtmService(nullptr){
  InitRTMService();
}

CAgoraRTMInstance::~CAgoraRTMInstance() {
  m_rtmService->release();
}

void CAgoraRTMInstance::setLoginWnd(HWND wnd) {
  m_loginWnd = wnd;
}

HWND CAgoraRTMInstance::getLoginWnd() {
  return m_loginWnd;
}

void CAgoraRTMInstance::setChatMsgWnd(HWND wnd) {
  m_ChatMsg = wnd;
}

HWND CAgoraRTMInstance::getChatMsgWnd() {
  return m_ChatMsg;
}

bool CAgoraRTMInstance::getptStatus() {
  return m_isLoginPt;
}

void CAgoraRTMInstance::setptStatus(bool status) {
  m_isLoginPt = status;
}

bool CAgoraRTMInstance::getChannelStatus() {
  return m_isJoinChannel;
}

void CAgoraRTMInstance::setChannleStatus(bool status) {
  m_isJoinChannel = status;
}


void CAgoraRTMInstance::setAppId(const std::string &appId) {
  m_AppId = appId;
}

std::string CAgoraRTMInstance::getAppId() {
  return m_AppId;
}

void CAgoraRTMInstance::setAppCertificateId(const std::string &certificateId) {
  m_AppCertificateId = certificateId;
}

std::string CAgoraRTMInstance::getCertificateId() {
  return m_AppCertificateId;
}


std::string CAgoraRTMInstance::getLoginAccount() {
  return m_Account;
}

IRtmService* CAgoraRTMInstance::getAgoraAPI()
{
  return m_rtmService;
}

bool CAgoraRTMInstance::Login(const std::string &account, const std::string & token) {
  if (m_rtmService != nullptr){
    m_Account = account;
    m_rtmService->login(token.c_str(), account.c_str());
    return true;
  }

  return false;
}

bool CAgoraRTMInstance::LogOut() {
  if (m_rtmService != nullptr){
    m_rtmService->logout();
    return true;
  }

  return false;
}

bool CAgoraRTMInstance::QueryIsOnline(const std::string &account) {
  return true;
}

bool CAgoraRTMInstance::sendInstantMsg(const std::string &account, const std::string &msg, bool bEnableOfflineMessage) {
  if (m_rtmService != nullptr){
    IMessage* rtmMessage(m_rtmService->createMessage());
    rtmMessage->setText(gbk2utf8(msg).c_str());
	agora::rtm::SendMessageOptions smo;
	smo.enableOfflineMessaging = bEnableOfflineMessage;
    int nret = m_rtmService->sendMessageToPeer(account.c_str(), rtmMessage,smo);
    rtmMessage->release();
    return true;
  }

  return false;
}

bool CAgoraRTMInstance::sendChannelMsg(const std::string &channel, const std::string &ChannelMsg, bool bEnableOfflineMessage) {
  if (m_Channel != nullptr){
    IMessage* rtmMessage(m_rtmService->createMessage());
    rtmMessage->setText(gbk2utf8(ChannelMsg).c_str());
	agora::rtm::SendMessageOptions smo;
	smo.enableOfflineMessaging = bEnableOfflineMessage;
    m_Channel->sendMessage(rtmMessage,smo);
    rtmMessage->release();
    return true;
  }

  return false;
}

bool CAgoraRTMInstance::ChannelQueryNumResult(const std::string &channelID) {
  if (m_Channel != nullptr){
    m_Channel->getMembers();
    return true;
  }

  return false;
}

bool CAgoraRTMInstance::JoinChannel(const std::string &channelID) {
  if (m_rtmService != nullptr){
    m_RtmCallback->setJoinChannel(channelID);
    IChannel* channel = m_rtmService->createChannel(channelID.c_str(), m_RtmCallback);
    m_Channel = channel;
    m_Channel->join();
    m_ChannelName = channelID;
    return true;
  }

  return false;
}

bool CAgoraRTMInstance::LeaveChannel() {
  if (m_rtmService != nullptr){
    m_Channel->leave();
    return true;
  }

  return false;
}

std::string CAgoraRTMInstance::getSDKVersion() {
  return getRtmSdkVersion();
}


bool CAgoraRTMInstance::SendImageMsg(const std::string &account, IImageMessage* message, bool bP2P)
{
    if (m_rtmService != nullptr) {
        SendMessageOptions options;
        int ret = -1;
        if (bP2P)
            ret = m_rtmService->sendMessageToPeer(account.c_str(), message, options);
        else
            ret = m_Channel->sendMessage(message);
        return ret == 0 ? true : false;
    }
    return false;
}


bool CAgoraRTMInstance::uploadImage(std::string filePath, long long& requestId)
{
    if (m_rtmService != nullptr) {
        int ret = m_rtmService->createImageMessageByUploading(filePath.c_str(), requestId);
        return ret == 0 ? true : false;
    }
    return false;
}

bool CAgoraRTMInstance::downloadImage(std::string filePath, std::string mediaId, long long& requestId)
{
    if (m_rtmService != nullptr) {
        int ret = m_rtmService->downloadMediaToFile(mediaId.c_str(), filePath.c_str(), requestId);
        return ret == 0 ? true : false;
    }
    return false;
}

bool CAgoraRTMInstance::CancelMediaUpload(long long requestId)
{
    if (m_rtmService != nullptr) {
        
        int ret = m_rtmService->cancelMediaUpload(requestId);
        return ret == 0 ? true : false;
    }
    return false;
}

bool CAgoraRTMInstance::CancelMediaDownload(long long requestId)
{
    if (m_rtmService != nullptr) {

        int ret = m_rtmService->cancelMediaDownload(requestId);
        return ret == 0 ? true : false;
    }
    return false;
}

void CAgoraRTMInstance::SetImageInfo(int w, int h, int tw, int th)
{
    m_RtmCallback->SetImageInfo(w, h, tw, th);
}