#include "menuwidget.h"
#include <QPainter>
#include <QLinearGradient>
#include <QResizeEvent>

MenuWidget::MenuWidget(QWidget *parent) : QWidget(parent)
{
    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("Введите никнейм...");
    nameEdit->setMaxLength(16);
    nameEdit->setAlignment(Qt::AlignCenter);
    nameEdit->setStyleSheet(R"(
        QLineEdit {
            background: rgba(255,255,255,18);
            border: 2px solid rgba(255,100,180,180);
            border-radius: 8px;
            color: white;
            font-size: 15px;
            padding: 6px 12px;
        }
        QLineEdit:focus {
            border-color: rgba(255,150,210,230);
        }
    )");

    startBtn = new QPushButton("▶  ИГРАТЬ", this);
    startBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #cc55aa, stop:1 #993377);
            border: 2px solid #ee77cc;
            border-radius: 10px;
            color: white;
            font-size: 17px;
            font-weight: bold;
            padding: 10px 30px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
                stop:0 #ee77cc, stop:1 #cc55aa);
        }
        QPushButton:pressed {
            background: #993377;
        }
    )");

    scoresLabel = new QLabel(this);
    scoresLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    scoresLabel->setStyleSheet("color: rgba(255,220,240,220); font-size: 13px;");
    scoresLabel->setText("Рекордов пока нет");
    scoresLabel->setWordWrap(false);

    nameEdit->setFixedWidth(240);
    startBtn->setFixedWidth(240);

    connect(startBtn, &QPushButton::clicked, this, [this]() {
        QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) name = "Игрок";
        emit startRequested(name);
    });
    connect(nameEdit, &QLineEdit::returnPressed, startBtn, &QPushButton::click);
}

void MenuWidget::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    layoutWidgets();
}

void MenuWidget::layoutWidgets()
{
    int w = width();
    int h = height();

    // Заголовок занимает верхние ~28% высоты
    int titleBottom = (int)(h * 0.28);

    // Никнейм под заголовком
    int editH = 38;
    int editY = titleBottom + 10;
    nameEdit->setGeometry((w - 240)/2, editY, 240, editH);

    // Кнопка под никнеймом
    int btnH = 46;
    int btnY = editY + editH + 12;
    startBtn->setGeometry((w - 240)/2, btnY, 240, btnH);

    // Таблица рекордов — остаток
    int tableY = btnY + btnH + 20;
    int tableH = h - tableY - 10;
    scoresLabel->setGeometry(10, tableY, w - 20, tableH);
}

void MenuWidget::refreshScores(const QVector<ScoreEntry> &scores)
{
    if (scores.isEmpty()) {
        scoresLabel->setText("Рекордов пока нет");
        return;
    }

    // Таблица в HTML
    QString html;
    html += "<table width='100%' cellspacing='0' cellpadding='4' "
            "style='border-collapse:collapse;'>";

    // Заголовок таблицы
    html += "<tr style='color:#ffaadd;font-size:12px;font-weight:bold;'>"
            "<td align='center' width='40'>#</td>"
            "<td align='left'>НИК</td>"
            "<td align='right'>ОЧКИ</td>"
            "</tr>";

    // Разделитель
    html += "<tr><td colspan='3'>"
            "<hr style='border:1px solid rgba(255,100,180,80);margin:2px 0;'/>"
            "</td></tr>";

    const QStringList medals = {"🥇","🥈","🥉"};

    for (int i = 0; i < scores.size(); ++i) {
        // Чередующийся фон строк
        QString rowBg = (i % 2 == 0)
                            ? "background:rgba(255,255,255,8);"
                            : "background:rgba(255,255,255,3);";

        QString medal = (i < 3)
                            ? medals[i]
                            : QString("<span style='color:#aaaaaa;'>%1</span>").arg(i+1);

        // Цвет ника для топ-3
        QString nameColor = "#ffffff";
        if      (i == 0) nameColor = "#ffd700";
        else if (i == 1) nameColor = "#c0c0c0";
        else if (i == 2) nameColor = "#cd7f32";

        html += QString(
                    "<tr style='%1'>"
                    "<td align='center' style='font-size:14px;'>%2</td>"
                    "<td align='left' style='color:%3;font-weight:bold;padding-left:8px;'>%4</td>"
                    "<td align='right' style='color:#ffddee;padding-right:8px;'>%5</td>"
                    "</tr>"
                    )
                    .arg(rowBg)
                    .arg(medal)
                    .arg(nameColor)
                    .arg(scores[i].name.toHtmlEscaped())
                    .arg(scores[i].score);
    }

    html += "</table>";

    // Заголовок над таблицей
    QString full = "<div align='center' style='color:#ffaadd;"
                   "font-size:13px;font-weight:bold;"
                   "margin-bottom:8px;'>🏆 &nbsp;ТАБЛИЦА РЕКОРДОВ</div>"
                   + html;

    scoresLabel->setText(full);
}

void MenuWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    // Фон
    QLinearGradient bg(0, 0, 0, height());
    bg.setColorAt(0.0, QColor(20, 10, 25));
    bg.setColorAt(1.0, QColor(8,   4, 15));
    p.fillRect(rect(), bg);

    // Тонкое свечение сверху
    QLinearGradient glow(0, 0, 0, 60);
    glow.setColorAt(0.0, QColor(255, 80, 160, 60));
    glow.setColorAt(1.0, QColor(255, 80, 160, 0));
    p.fillRect(0, 0, width(), 60, glow);

    // Заголовок TETRIS
    int titleBottom = (int)(height() * 0.28);
    QRect titleRect(0, 0, width(), titleBottom);

    p.setFont(QFont("Arial", 54, QFont::Bold));
    QLinearGradient tg(0, titleRect.top(), 0, titleRect.bottom());
    tg.setColorAt(0.0, QColor(255, 180, 220));
    tg.setColorAt(0.4, QColor(255,  80, 160));
    tg.setColorAt(1.0, QColor(180,  20, 100));
    p.setPen(QPen(QBrush(tg), 1));
    p.drawText(titleRect, Qt::AlignHCenter | Qt::AlignBottom, "TETRIS");

    // Разделитель под заголовком
    p.setPen(QPen(QColor(255, 80, 160, 100), 1));
    int lineY = titleBottom + 4;
    p.drawLine(width()/2 - 100, lineY, width()/2 + 100, lineY);
}
