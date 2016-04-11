#include "widget.h"
#include "ui_widget.h"
#include <QTimer>
#include <QDebug>
#include "math.h"

static uchar runFlag, runMode;

//赛道变量
#define RACE2
static uchar race[800][800];

//运动变量
const int xBegin = 350, yBegin = 335;
static int xBase, yBase;
static double xFix, yFix;
static double sita;
static double step, vYaw;

static int turn = 0;

//图片显示中间变量
const unsigned int ovSize = 240;
static unsigned int ovArea = ovSize * ovSize;
static unsigned int *pImage = (unsigned int*)malloc(ovArea*sizeof(unsigned int));

//图片结果变量
 static uchar disData[80][80];

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    uchar raceMode[80][80] = {0};
    #ifdef RACE1
    for (int i = 30; i < 50; ++i)
    {
        raceMode[i][30] = 1;
        raceMode[i][49] = 1;
        raceMode[30][i] = 1;
        raceMode[49][i] = 1;

    }
    #endif
    #ifdef RACE2
    for(int i = 0; i < 10; i++)
    {
        raceMode[40-5][61-15+i] = 1;
        raceMode[40+5][61-15+i] = 1;
        raceMode[40-5-i][40-5] = 1;
        raceMode[40-5-i][40+5] = 1;
        raceMode[40+5+i][40-5] = 1;
        raceMode[40+5+i][40+5] = 1;
    }
    for(int i = 0; i < 11; i++)
    {
        raceMode[35+i][35] = 1;
        raceMode[35+i][55] = 1;
        raceMode[25][35+i] = 1;
        raceMode[55][35+i] = 1;
    }
    #endif
    for(int i = 10; i < 70; i++)
    {
        raceMode[i][10] = 1;
        raceMode[i][69] = 1;
        raceMode[10][i] = 1;
        raceMode[69][i] = 1;
    }

    //生成赛道
    for(int i = 0; i < 80; i++)
    {
        for(int j = 0; j < 80; j++)
        {
            for(int k = i*10; k < (i+1)*10; k++)
            {
                for(int l = j*10; l < (j+1)*10; l++)
                {
                    race[k][l] = raceMode[i][j];
                }
            }
        }
    }

    ui->comboBox->addItem("  手 动");
    ui->comboBox->addItem(" 半自动");
    ui->comboBox->addItem(" 全自动");

    ui->vEdit->setValidator(new QIntValidator(0, 99, this));
    ui->wEdit->setValidator(new QIntValidator(0, 90, this));

    ui->pushButton->setEnabled(true);
    ui->pushButton_UP->setEnabled(false);
    ui->pushButton_DN->setEnabled(false);
    ui->pushButton_L->setEnabled(false);
    ui->pushButton_R->setEnabled(false);
    ui->pushButton_EL->setEnabled(false);
    ui->pushButton_ER->setEnabled(false);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()) , this, SLOT(updateNomal()));
}

Widget::~Widget()
{
    delete ui;
}

//启动：正常运行
//停止：“base坐标”回到原点，“绝对方向sita” 归零，“速度step”归零
void Widget::on_pushButton_clicked()
{
    xBase = xBegin; yBase = yBegin;
    if(runFlag == 0)
    {
        runFlag = 1;
        ui->pushButton->setText("停止");
        if(ui->comboBox->currentIndex() != 2 )
        {
            ui->pushButton_UP->setEnabled(true);
            ui->pushButton_DN->setEnabled(true);
            ui->pushButton_EL->setEnabled(false);
            ui->pushButton_ER->setEnabled(true);
            if(ui->comboBox->currentIndex() != 1)
            {
                ui->wEdit->setText("18");
                ui->pushButton_L->setEnabled(true);
                ui->pushButton_R->setEnabled(true);
            }
        }
        else
        {
            ui->vEdit->setText("6");
        }

        if(ui->comboBox->currentIndex() != 0 )
            runMode = 1;
        else
            runMode = 0;

        ui->comboBox->setDisabled(true);
        timer->start(200);
    }
    else
    {
        runFlag = 0;
        timer->stop();
        ui->label->clear();
        sita = 0.00;
        step = 0.00;
        xFix = 0.00; yFix = 0.00;
        turn = 0;
        ui->pushButton->setText("启动");
        ui->pushButton_UP->setEnabled(false);
        ui->pushButton_DN->setEnabled(false);
        ui->pushButton_L->setEnabled(false);
        ui->pushButton_R->setEnabled(false);
        ui->pushButton_EL->setEnabled(false);
        ui->pushButton_ER->setEnabled(false);
        ui->vEdit->setText("0");
        ui->wEdit->setText("0");
        ui->sEdit->setText("0");
        ui->comboBox->setDisabled(false);
    }
}

//加速：增大“速度step”
void Widget::on_pushButton_UP_clicked()
{
    if(step < 50)
    {
        step+=2;
        QString s = QString::number(step);
        ui->vEdit->setText(s);
    }
}

//减速：减小“速度step”
void Widget::on_pushButton_DN_clicked()
{
    if(step >= 2)
    {
        step-=2;
        QString s = QString::number(step);
        ui->vEdit->setText(s);
    }
}

//左转：减小“绝对方向sita”
void Widget::on_pushButton_R_clicked()
{
    vYaw = ui->wEdit->text().toDouble();
    sita -= vYaw;
}

//右转：增大“绝对方向sita”
void Widget::on_pushButton_L_clicked()
{
    vYaw = ui->wEdit->text().toDouble();
    sita += vYaw;
}

//正常运行：每0.1s动作一次
void Widget::updateNomal()
{
    getImage();
    doYourAIGO();
    goNomal();
    disImage();
}

//获取图像：二值、80*80的disData
void Widget::getImage()
{
    uchar tempData[180][180] = {0};
    uchar imgData[80][80] = {0};
    double sinSita = 0.00, cosSita = 0.00;
    double xt = 0, yt = 0;
    int xi = 0, yi = 0;

    sinSita = sin(3.1416 * sita / 180.00);
    cosSita = cos(3.1416 * sita / 180.00);

    //所在120*120地面旋转Sita度，即为实际图像
    for(int i = 0; i < 120; i++)
    {
        for(int j = 0; j < 120; j++)
        {
            //旋转公式
            xt = ( (i - 60) * cosSita - (j - 60) * sinSita );
            yt = ( (i - 60) * sinSita + (j - 60) * cosSita );

            //结果四舍五入
            if(xt >= 0)
                xi = (int)(xt + 0.5);
            else
                xi = (int)(xt - 0.5);
            if(yt >= 0)
                yi = (int)(yt + 0.5);
            else
                yi = (int)(yt - 0.5);

            //tempData中心坐标修正90, race修正40
            tempData[xi + 90][yi + 90] = race[i+xBase-40][j+yBase-40];
        }
    }

    //在120*120图中选取中心的80*80作为结果图像
    for(int i = 0; i < 80;  i++)
    {
        for(int j = 0; j < 80; j++)
        {
            disData[i][j] = 0;
            imgData[i][j] = tempData[i+50][j+50];
        }
    }

    //膨胀操作，消除杂点
    for(int i = 1; i < 79; i++)
    {
        for(int j = 1; j < 79; j++)
        {
            disData[i][j] = 0;
            if(imgData[i-1][j-1] || imgData[i-1][j] || imgData[i-1][j+1] ||
               imgData[i][j-1]   || imgData[i][j]   || imgData[i][j+1]   ||
               imgData[i+1][j-1] || imgData[i+1][j] || imgData[i+1][j+1]  )
            {
                disData[i][j] = 1;
            }
        }
    }
}

//自定义循迹算法：disData->循迹算法->更新“绝对方向sita”和“速度step”
void Widget::doYourAIGO()
{
    uchar edge[80][80] = {0};
    int i = 0, j = 0;

    // ------------------------------- 边缘提取 ---------------------------------------
    for(i = 2; i < 78; i++)
    {
        for(j = 2; j < 78; j++)
        {
            if(disData[i][j] != disData[i][j+1] ||
               disData[i][j] != disData[i+1][j])
            {
                edge[i][j] = 3;
            }
        }
    }

    // ------------------------------ 寻找链码起点 --------------------------------------
    int flag = 0;
    int leftBeginX = 40, leftBeginY = 40, rightBeginX = 40, rightBeginY = 40;

    for(i = 77; i > 1; i--)
    {
        flag = 0;

        for(j = 77; j > 1; j--)
        {
            if(edge[i][j] == 3)
            {
                flag++;
                rightBeginY = j;
                break;
            }
        }
        for(j = 2; j < 78; j++)
        {
            if(edge[i][j] == 3)
            {
                flag++;
                leftBeginY = j;
                break;
            }
        }
        if(flag == 2 && (rightBeginY-leftBeginY) > 4)
        {
            leftBeginX = i;
            rightBeginX = i;
            break;
        }
    }
    edge[leftBeginX][leftBeginY] = 230;
    int rightX = rightBeginX, rightY = rightBeginY;
    int leftX = leftBeginX, leftY = leftBeginY;

    // ------------------------------- Left链码 ---------------------------------------
    int n = 0, last = 0, now = 0;
    int find[8][2] = {{-1,0},{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1}};
    while(n < 160)
    {
        for(i = 0; i < 8; i++)
        {
            now = last + i;

            if(now < 0)
                now += 8;
            if(now > 7)
                now -= 8;

            if(edge[leftX+find[now][0]][leftY+find[now][1]] == 3)
            {
                leftX = leftX + find[now][0];
                leftY = leftY + find[now][1];
                edge[leftX][leftY] = 2;
                break;
            }
        }
        last = now - 2;
        n++;
    }
    edge[leftX][leftY] = 250;

    // ------------------------------- Right链码 ---------------------------------------
    n = 0, last = 0, now = 0;
    while(n < 160)
    {
        for(i = 0; i < 8; i++)
        {
            now = last + i;

            if(now < 0)
                now += 8;
            if(now > 7)
                now -= 8;
            if(edge[rightX+find[now][0]][rightY+find[now][1]] == 3)
            {
                rightX = rightX + find[now][0];
                rightY = rightY + find[now][1];
                edge[rightX][rightY] = 2;
                break;
            }
        }
        last = now - 2;
        n++;
    }
    edge[rightX][rightY] = 250;

    //have: rightX rightY leftX leftY and their begin.
    //set : sita step
    //attention : start - end
    //--------------------------------  线路决策 ----------------------------------------
    //目前用按钮进行决策

    //--------------------------------  姿态调整  ---------------------------------------
    double tt = 0.0;
    if(turn)
    {
        edge[rightX-1][rightY-1] = 250; edge[rightX-1][rightY+1] = 250;
        edge[rightX+1][rightY-1] = 250; edge[rightX+1][rightY+1] = 250;
        tt= atan((double)(rightY-40)/(double)(70-rightX))/3.1416*180;
    }
    else
    {
        edge[leftX-1][leftY-1] = 250; edge[leftX-1][leftY+1] = 250;
        edge[leftX+1][leftY-1] = 250; edge[leftX+1][leftY+1] = 250;
        tt= atan((double)(leftY-40)/(double)(70-leftX))/3.1416*180;
    }
    if(runMode != 0)
    {       
        sita += tt;
        ui->wEdit->setText(QString::number(tt, 10, 1));
    }

    //------------------------------  图像信息加注  ---------------------------------------
    for(i = 0; i < 80; i++)
    {
        for(j = 0; j < 80; j++)
        {
            if(edge[i][j] != 0)
                disData[i][j] = edge[i][j];
        }
    }
}

//正常飞行：从 “base坐标” 向 “sita角度” 飞行 “step距离”
void Widget::goNomal()
{
    int temp = 0;
    double ans = 0.00;
    double sinSita = 0.00, cosSita = 0.00;
    sinSita = sin(3.1416 * sita / 180.00);
    cosSita = cos(3.1416 * sita / 180.00);

    step = ui->vEdit->text().toInt();

    if(sita >= 360)
    {
        sita -= 360;
    }
    else if(sita < 0)
    {
        sita += 360;
    }
    ui->sEdit->setText(QString::number(sita, 10, 1));

    ans = cosSita*step + xFix;
    temp = (int)(ans);
    if(temp >= 0)
        temp += 0.5;
    else
        temp -= 0.5;
    if(xBase-temp > 100 && xBase-temp < 700)
    {
        xBase -= temp;
        xFix = ans - (double)temp;
    }

    ans = sinSita*step + yFix;
    temp = (int)(ans);
    if(temp >= 0)
        temp += 0.5;
    else
        temp -= 0.5;

    if(yBase+temp > 100 && yBase+temp < 700)
    {
        yBase += temp;
        yFix = ans - (double)temp;
    }
}

//显示图像：disData
void Widget::disImage()
{
    //图像显示
    int n = 0;
    uchar *pa = NULL,*pb = NULL,*pc = NULL;
    for (int i = 0; i < 80; ++i)
    {
        for(int k = 0; k < 3; ++k)
        {
            for (int j = 0; j < 80; ++j)
            {
                pa = (uchar *)(pImage+n);
                pb = (uchar *)(pImage+n+1);
                pc = (uchar *)(pImage+n+2);

                pImage[n] = 0;pImage[n+1] = 0;pImage[n+2] = 0;
                if(disData[i][j] == 1)
                {
                    pa[0] = 255; pa[1] = 255; pa[2] = 255;
                    pb[0] = 255; pb[1] = 255; pb[2] = 255;
                    pc[0] = 255; pc[1] = 255; pc[2] = 255;
                }
                else if(disData[i][j] == 2)
                {
                    pa[0] = 0; pa[1] = 255; pa[2] = 0;
                    pb[0] = 0; pb[1] = 255; pb[2] = 0;
                    pc[0] = 0; pc[1] = 255; pc[2] = 0;
                }
                else if(disData[i][j] == 3)
                {
                    pa[0] = 0; pa[1] = 255; pa[2] = 255;
                    pb[0] = 0; pb[1] = 255; pb[2] = 255;
                    pc[0] = 0; pc[1] = 255; pc[2] = 255;
                }
                else
                {
                    pa[0] = 0; pa[1] = 0; pa[2] = disData[i][j];
                    pb[0] = 0; pb[1] = 0; pb[2] = disData[i][j];
                    pc[0] = 0; pc[1] = 0; pc[2] = disData[i][j];
                }
                n+=3;   
            }
        }
    }

    QByteArray imageByteArray = QByteArray( (const char*)pImage,  ovArea*4 );
    uchar*  transData = (uchar*)imageByteArray.data();
    QImage image = QImage(transData, ovSize, ovSize, QImage::Format_RGB32);
    ui->label->setPixmap(QPixmap::fromImage(image));
}

void Widget::on_pushButton_EL_clicked()
{
    turn = 0;
    ui->pushButton_EL->setEnabled(false);
    ui->pushButton_ER->setEnabled(true);
}

void Widget::on_pushButton_ER_clicked()
{
    turn = 1;
    ui->pushButton_EL->setEnabled(true);
    ui->pushButton_ER->setEnabled(false);
}
