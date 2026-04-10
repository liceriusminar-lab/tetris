#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Тетрис");

    stack = new QStackedWidget(this);
    setCentralWidget(stack);

    menu = new MenuWidget(this);
    game = new TetrisWidget(this);
    api  = new ApiClient(this);

    stack->addWidget(menu);
    stack->addWidget(game);

    connect(menu, &MenuWidget::startRequested, this, &MainWindow::showGame);
    connect(game, &TetrisWidget::gameFinished, this, &MainWindow::onGameOver);

    // Когда пришли рекорды с сервера — обновляем меню
    connect(api, &ApiClient::scoresReceived, menu, &MenuWidget::refreshScores);

    resize(TetrisWidget::BOARD_WIDTH  * TetrisWidget::BLOCK_SIZE + 160,
           TetrisWidget::BOARD_HEIGHT * TetrisWidget::BLOCK_SIZE);

    // Загружаем рекорды при старте
    api->fetchScores();

    showMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showMenu()
{
    api->fetchScores(); // обновляем каждый раз при входе в меню
    stack->setCurrentIndex(0);
}

void MainWindow::showGame(const QString &nickname)
{
    currentPlayer = nickname;
    stack->setCurrentIndex(1);
    game->startGame();
}

void MainWindow::onGameOver(int score)
{
    // Отправляем результат на сервер — он сам обновит таблицу
    api->postScore(currentPlayer, score);
    showMenu();
}
