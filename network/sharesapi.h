#ifndef SHARESAPI_H
#define SHARESAPI_H

#include <QString>

struct SharedFileModel {
    QString share_id;
    QString file_id;
    QString file_name;
    QString file_type;
    int file_size;
    QString shared_by;
    QString shared_by_username;
    QString created_at;
};

struct ShareDetailModel {
    QString share_id;
    QString shared_with;
    QString shared_with_username;
    QString created_at;
    QString file_id;
    QString file_name;
    int file_size;
    QString file_type;
};



class SharesApi
{
public:
    SharesApi();
    static bool shareEncryptedFile(const QString& fileId,
                                   const QString& authToken,
                                    const QString& sharedWithUsername,
                                    const QString& encryptedFile,
                                    const QString& nonce,
                                    const QString& ephemeralPublicKey,
                                    const QString& signature,
                                    QString& responseOut,
                                    QString& errorOut);

    static bool downloadSharedFile(const QString& shareId,const QString& authToken, QString& responseOut, QString& errorOut);
    static bool getPublicKeysForUser(const QString& username,const QString& authToken, QString& responseOut, QString& errorOut);
    static bool getFilesSharedWithMe(QList<SharedFileModel>& filesOut,const QString& authToken, QString& errorOut);
    static bool getSharesForFile(const QString& file_id,const QString& authToken, QList<ShareDetailModel>& sharesOut, QString& errorOut);
    static bool revokeShareAccess(const QString& file_id,const QString& authToken, const QString& shared_with_username, QString& errorOut);

};

#endif // SHARESAPI_H
