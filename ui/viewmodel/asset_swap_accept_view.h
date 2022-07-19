// Copyright 2022 The Beam Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include <QObject>

class AssetSwapAcceptViewModel: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString amountToReceive   READ getAmountToReceive   NOTIFY  orderChanged)
    Q_PROPERTY(QString amountToSend      READ getAmountToSend      NOTIFY  orderChanged)
    Q_PROPERTY(QString fee               READ getFee               NOTIFY  orderChanged)
    Q_PROPERTY(QString offerCreated      READ getOfferCreated      NOTIFY  orderChanged)
    Q_PROPERTY(QString offerExpires      READ getOfferExpires      NOTIFY  orderChanged)
    Q_PROPERTY(QString comment           READ getComment           WRITE   setComment   NOTIFY  commentChanged)
    Q_PROPERTY(QString rate              READ getRate              NOTIFY  orderChanged)

  public:
    AssetSwapAcceptViewModel();

  signals:
    void orderChanged();
    void commentChanged();

//   private slots:
//     void onGeneratedNewAddress(const beam::wallet::WalletAddress& walletAddr);

  private:
    QString getAmountToReceive() const;
    QString getAmountToSend() const;
    QString getFee() const;
    QString getOfferCreated() const;
    QString getOfferExpires() const;

    QString getComment() const;
    void setComment(QString value);

    QString getRate() const;

    QString   _comment;
};
