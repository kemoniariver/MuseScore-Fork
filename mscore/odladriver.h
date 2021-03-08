﻿#ifndef ODLADRIVER_H
#define ODLADRIVER_H

#include <QObject>
#include <QLocalSocket>
#include "scoreview.h"
#include "libmscore/spanner.h"
#include "libmscore/select.h"

namespace ODLA {

/*!
 * \brief The ODLADriver class
 */
class ODLADriver : public QObject
{
    enum messageType: char
    {
        NO_ELEMENT,
        SINGLE_ELEMENT,
        RANGE_ELEMENT,
    };

    union state_message_t
    {
        struct common_fields_t
        {
            quint8 msgLen;
            messageType type;
            Ms::ScoreState mscoreState;
            Ms::SelState selectionState;
            int selectedElements;
        } common_fields;

        struct element_fields_t
        {
            struct common_fields_t common_fields;
            Ms::ElementType elementType;
            quint8 notePitch;
            Ms::AccidentalType noteAccident;
            Ms::TDuration::DurationType duration;
            quint8 dotsNum;
            int measureNum;
            quint8 beat;
            quint8 staff;
            Ms::ClefType clef;
            quint8 timeSignatureNum;
            quint8 timeSignatureDen;
            Ms::Key keySignature;
            quint8 voiceNum;
            int bpm;
        } element_fields;

        struct range_fields_t
        {
            struct common_fields_t common_fields;
            int firstMesaure;
            int lastMesaure;
            quint8 firstBeat;
            quint8 lastBeat;
            quint8 firstStaff;
            quint8 lastStaff;
        } range_fields;

        char data[sizeof(element_fields_t)];
    };

    Q_OBJECT
public:
    explicit ODLADriver(QObject *parent = nullptr);

    void setScoreView(Ms::ScoreView* scoreView);

    static QString nextValidFileName(QString prefix, QString ext = "mscx");
    static QString nextUntitledSuffixNumber(QString untitledNamePrefix, QString ext = "mscx");

signals:

public slots:
    void attemptConnection();
    void setCurrentScore(Ms::MasterScore* current);

protected slots:
    void onConnected();
    void onIncomingData();
    void collectAndSendStatus();

protected:
    QLocalSocket* _localSocket;
    Ms::MasterScore* _currentScore;
    Ms::ScoreView* _scoreView;
    QString _untitledPrefix;

    bool _editingChord;

    void setNoteEntryMode(bool enabled);
    bool addSpannerToCurrentSelection(Ms::Spanner* spanner);

    quint8 getNotePitch(Ms::Element *e);
    Ms::AccidentalType getNoteAccident(Ms::Element *e);
    Ms::TDuration::DurationType getDuration(Ms::Element *e);
    quint8 getDots(Ms::Element *e);
    int getMeasureNumber(Ms::Element *e);
    quint8 getBeat(Ms::Element *e);
    quint8 getStaff(Ms::Element *e);
    Ms::ClefType getClef(Ms::Element *e);
    Ms::Fraction getTimeSig(Ms::Element *e);
    Ms::Key getKeySignature(Ms::Element *e);
    quint8 getVoice(Ms::Element *e);
    int getBPM(Ms::Element *e);

    Ms::Element* findElementBefore(Ms::Element *el, Ms::ElementType type, int staffIdx = -1);
    Ms::Element* findElementParent(Ms::Element *el, Ms::ElementType type);
    QTimer *_reconnectTimer;
};

} // namespace ODLA

#endif // ODLADRIVER_H
