#ifndef IBROWSER_H
#define IBROWSER_H
#include "engine_global.h"

#include <QObject>
#include <QList>
#include "iwebelement.h"
#include "inetworkaccessmanager.h"
#include "inetworkaccessmanagerfactory.h"

namespace BrowserAutomationStudioFramework
{
    class IWebElement;
    class IScriptResources;
    class IWorker;
    class INetworkAccessManager;
    class INetworkAccessManagerFactory;
    class ENGINESHARED_EXPORT IBrowser : public QObject
    {
    Q_OBJECT
    public:
        explicit IBrowser(QObject *parent = 0);


    signals:
    public slots:
        virtual void LoadPage(const QString& url, const QString& callback) = 0;
        virtual void SetUserAgent(const QString& agent, const QString& callback) = 0;
        virtual void SetNetworkAccessManagerFactory(INetworkAccessManagerFactory *NetworkAccessManagerFactory) = 0;
        virtual void GetCookiesForUrl(const QString& url, const QString& callback) = 0;
        virtual void SaveCookies(const QString& callback) = 0;
        virtual void RestoreCookies(const QString& cookies, const QString& callback) = 0;
        virtual IWebElement* GetRootElement() = 0;
        virtual void GetUrl(const QString& callback) = 0;
        virtual void CreateNewBrowser(bool ForseNewBrowserCreation, const QString& callback) = 0;
        virtual INetworkAccessManager* GetNetworkAccessManager() = 0;
        virtual void SetScriptResources(IScriptResources* ScriptResources) = 0;
        virtual IScriptResources* GetScriptResources() = 0;
        virtual void SetWorker(IWorker* Worker) = 0;
        virtual IWorker* GetWorker() = 0;
        virtual void SetOpenFileName(const QString & OpenFileName, const QString& callback) = 0;
        virtual void MouseClickInstant(int x, int y) = 0;
        virtual void MouseClick(int x, int y, const QString& callback) = 0;
        virtual void MouseMove(int x, int y, const QString& callback) = 0;
        virtual void Scroll(int x, int y, const QString& callback) = 0;
        virtual void Render(int x, int y, int width, int height, const QString& callback) = 0;

        virtual void Resize(int x, int y, const QString& callback) = 0;
        virtual void Reset(const QString& callback) = 0;
        virtual void Jquery(const QString& callback) = 0;
        virtual void OptimizeMemory(const QString& callback) = 0;
        virtual void WaitCode(const QString& callback) = 0;
        virtual void StartSection(const QString& Name, int Id, const QString& callback) = 0;

        virtual void OnSupend() = 0;
    };

}

#endif // IBROWSER_H
