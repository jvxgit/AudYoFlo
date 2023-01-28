#include <QCoreApplication>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
 
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
 
	/* http://doc.qt.io/qt-5/qtwebsockets-echoclient-example.html */
    
	QEventLoop eventLoop;
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
	QNetworkRequest req_get(QUrl(QString("http://137.226.198.65:8000/jvx/host/components/return?component=audio technology")));
	QNetworkReply *reply = mgr.get(req_get);

	/*
	QNetworkRequest req_post(QUrl(QString("http://137.226.198.65:8000/jvx/host/components/")));
	req.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
	QByteArray someData = "purpose=description&component_target=audio+technology&property_descriptor=/xyz";
    QNetworkReply *reply = mgr.post(req, someData);
	*/
	
    eventLoop.exec();
 
    if (reply->error() == QNetworkReply::NoError) {
        QString strReply = (QString)reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
        QJsonObject jsonObj = jsonResponse.object();
		QJsonObject::iterator elm = jsonObj.begin();
		for (; elm != jsonObj.end(); elm++)
		{
			qDebug() << elm.key() << "=" << elm.value();
		}
      
        delete reply;
    }
    else {
        qDebug() << "Failure" <<reply->errorString();
        delete reply;
    }
    return 0;
}