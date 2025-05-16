#include "noticewidget.h"

NoticeWidget::NoticeWidget(QWidget *parent)
    : mParentPtr(parent),
      mTimerPtr(nullptr),
      mTimerCount(NOTICE_DEF_DELAY_CNT),
      mBaseWidth(0),
      mBaseHeight(0),
      mMinHeight(0),
      mTransparentVal(TRANSPARENT_MAX_VAL) {
    setAlignment(Qt::AlignCenter);  // 文字居中

    mTimerPtr = new QTimer();  // 定时器，定时消失
    connect(mTimerPtr, SIGNAL(timeout()), this, SLOT(OnTimerTimeout()), Qt::UniqueConnection);
}

NoticeWidget::~NoticeWidget() {
    if (mTimerPtr->isActive()) {
        mTimerPtr->stop();
    }
    delete mTimerPtr;
    deleteLater();
}

void NoticeWidget::OnTimerTimeout() {
    --mTimerCount;
    if (0 < mTimerCount) {
        if (nullptr != mParentPtr) {  // 重新定位(窗口大小和位置可能变化)
            QPoint pt((mParentPtr->width() - width()) >> 1, (mParentPtr->height() - height()) >> 1);
            if (pos() != pt) {  // 父窗口位置变化
                ChangeSize();
                move(pt);
            }
        }
        if (mTimerCount <= NOTICE_DEF_DELAY_CNT && 0 < mTransparentVal) {  // 最后1s开始渐变消失
            mTransparentVal -= TRANSPARENT_CUT_VAL;
            if (0 > mTransparentVal) {
                mTransparentVal = 0;
            }
            setStyleSheet(QString(STYLE_SHEET).arg(mTransparentVal));  // 控制透明度
        }
    } else {  // 显示结束
        mTimerPtr->stop();
        setVisible(false);
    }
}

// 设置要显示的消息
void NoticeWidget::SetMesseage(const QString &msg, int delay_ms) {
    QStringList strList = msg.split("\n");
    QFontMetrics fontMetrics(font());
    mListLinesLen.clear();

    int tmpW = 0;
    int maxLineLen = 1;  // 最长那一行的长度
    foreach (QString s, strList) {
        tmpW = fontMetrics.width(s);
        mListLinesLen.append(tmpW);
        if (maxLineLen < tmpW) {
            maxLineLen = tmpW;
        }
    }

    mParentPtr = parentWidget();
    mBaseWidth = fontMetrics.width(msg);
    mBaseHeight = fontMetrics.lineSpacing() + PATCH_HEIGHT;
    mMinHeight = (mBaseWidth * mBaseHeight) / maxLineLen + 1;  // 面积除以最长的宽就是最小的高

    ChangeSize();                 // 设置宽高
    setWordWrap(true);            // 换行
    setText(msg);                 // 设置显示内容
    if (nullptr != mParentPtr) {  // 居中
        move((mParentPtr->width() - width()) >> 1, (mParentPtr->height() - height()) >> 1);
    }

    setVisible(true);                                              // 显示
    setStyleSheet(QString(STYLE_SHEET).arg(TRANSPARENT_MAX_VAL));  // 设置样式，不透明
    mTimerCount = delay_ms / TIMER_INTERVAL_MS + 1;                // 延时计数计算
    mTransparentVal = TRANSPARENT_MAX_VAL;
}

// 跟随父窗口大小变化
void NoticeWidget::ChangeSize() {
    if (nullptr != mParentPtr) {
        double wd = mParentPtr->width() * SIZE_SCALE;  // 宽度占父窗口的80%
        // 提示内容多少决定提示框面积，长方形面积s=mBaseHeight*mBaseWidth
        // 面积s固定，当mBaseWidth跟随窗体宽度变大而增大，那么mBaseHeight应当变小，才维持s固定
        int newH = (mBaseHeight * mBaseWidth) / wd + PATCH_HEIGHT;
        if (newH < (mMinHeight + mBaseHeight)) {  // 设定最小高度
            newH = mMinHeight + mBaseHeight;
        } else {
            foreach (int lineLen, mListLinesLen) {
                if (lineLen > wd) {  // 某一行长度大于当前宽度就会发生折行，高度需要增加
                    newH += mBaseHeight;
                }
            }
        }
        setFixedSize((int)wd, newH);
    }
}

// 显示消息，可通过设置delay_ms=0来立即关闭显示
void NoticeWidget::Notice(QWidget *parent, const QString &msg, const int delay_ms) {
    if (mTimerPtr->isActive()) {
        mTimerPtr->stop();
        setVisible(false);
    }
    if (msg.isEmpty() || 0 >= delay_ms) {  // 消息为空直接返回
        return;
    }

    setParent(parent);
    SetMesseage(msg, delay_ms);
    mTimerPtr->start(TIMER_INTERVAL_MS);  // 开始计数
}
