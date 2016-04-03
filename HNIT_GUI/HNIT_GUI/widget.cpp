#include "widget.h"
#include "ui_widget.h"
#include <QTimer>
#include <QDebug>
#include "math.h"

uchar runFlag = 0;
uchar runModeN = 0;

QString auto0P = "  手 动";
QString auto50P = " 半自动";
QString auto100P = " 全自动";

//摄像、赛道变量
uchar imgData[80][80] = {0};
uchar raceMode[80][80] = {0};
uchar raceData[800][800] = {0};

//运动变量
int xBase = 300, yBase = 300;
double xFix = 0.00, yFix = 0.00;
double step = 0.00;
double sita = 0.00;
double vSita = 0.00;
double sinSita = 0.00, cosSita = 0.00;

//图片结果变量
uchar disData[80][80] = {0};

//图片显示中间变量
const int ovSize = 240;
unsigned int ovArea = ovSize * ovSize;
unsigned int *pImage = (unsigned int*)malloc(ovArea*sizeof(unsigned int));


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //设计赛道
    for (int i = 30; i < 50; ++i)
    {
        raceMode[i][30] = 1;
        raceMode[i][49] = 1;
        raceMode[30][i] = 1;
        raceMode[49][i] = 1;

    }
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
                    raceData[k][l] = raceMode[i][j];
                }
            }
        }
    }

    ui->comboBox->addItem(auto0P);
    ui->comboBox->addItem(auto50P);
    ui->comboBox->addItem(auto100P);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()) , this, SLOT(updateNomal()));

    ui->vEdit->setValidator(new QIntValidator(0, 99, this));
    ui->wEdit->setValidator(new QIntValidator(0, 90, this));

    ui->pushButton->setEnabled(true);
    ui->pushButton_UP->setEnabled(false);
    ui->pushButton_DN->setEnabled(false);
    ui->pushButton_L->setEnabled(false);
    ui->pushButton_R->setEnabled(false);
}

Widget::~Widget()
{
    delete ui;
}

//启动：正常运行
//停止：“base坐标”回到原点，“绝对方向sita” 归零，“速度step”归零
void Widget::on_pushButton_clicked()
{
    if(runFlag == 0)
    {
        runFlag = 1;
        ui->pushButton->setText("停止");
        if(ui->comboBox->currentIndex() != 2 )
        {
            ui->pushButton_UP->setEnabled(true);
            ui->pushButton_DN->setEnabled(true);
            ui->pushButton_L->setEnabled(true);
            ui->pushButton_R->setEnabled(true);
        }
        if(ui->comboBox->currentIndex() != 0 )
            runModeN = 1;
        else
            runModeN = 0;

        ui->comboBox->setDisabled(true);
        timer->start(200);
    }
    else
    {
        runFlag = 0;
        timer->stop();
        ui->label->clear();
        xBase = 300; yBase = 300;
        sita = 0.00;
        step = 0.00;
        xFix = 0.00; yFix = 0.00;
        ui->pushButton->setText("启动");
        ui->pushButton_UP->setEnabled(false);
        ui->pushButton_DN->setEnabled(false);
        ui->pushButton_L->setEnabled(false);
        ui->pushButton_R->setEnabled(false);
        ui->vEdit->setText("0");
        ui->wEdit->setText("18");
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
    vSita = ui->wEdit->text().toDouble();
    sita -= vSita;
}

//右转：增大“绝对方向sita”
void Widget::on_pushButton_L_clicked()
{
    vSita = ui->wEdit->text().toDouble();
    sita += vSita;
}

//正常运行：每0.1s动作一次
void Widget::updateNomal()
{
    getImage();
    if(runModeN != 0)
        doYourAIGO();
    goNomal();
    disImage();
}

//获取图像：二值、80*80的disData
void Widget::getImage()
{
    uchar tempData[180][180] = {0};
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

            //tempData中心坐标修正90, raceData修正40
            tempData[xi + 90][yi + 90] = raceData[i+xBase-40][j+yBase-40];
        }
    }

    //在120*120图中选取中心的80*80作为结果图像
    for(int i = 0; i < 80;  i++)
    {
        for(int j = 0; j < 80; j++)
        {
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
    int cout = 0, sum = 0, lastSum = 0, min = 0;

    int ans = 0;

    for(int i = 60; i > 0; i--)
    {
        cout = 0;
        sum = 0;
        for(int j = 0; j < 80; j++)
        {
            if(disData[i][j] == 1)
            {
                cout++;
                sum += j;
            }
        }

        if(cout != 0)
        {
            sum/=cout;
            if(i==60)
                lastSum = sum;

            if(sum> 0 && sum < 80)
            {
                ans+=sum;
                disData[i][sum] = 2;
                min = min + sum - lastSum;
            }
            lastSum = sum;
        }
    }

    if(min > 10)
    {
        sita += 18;
    }
    else if(min < -10)
    {
        sita -= 18;
    }
    else if(ans/60 < 35)
    {
        double t = cos(sita);
        xBase += (int)(t*10);
        t = sin(sita);
        yBase -= (int)(t*10);
    }
    else if(ans/60 > 45)
    {
        double t = cos(sita);
        xBase -= (int)(t*10);
        t = sin(sita);
        yBase += (int)(t*10);
    }
    else
    {
        ;
    }

}

//正常飞行：从 “base坐标” 向 “sita角度” 飞行 “step距离”
void Widget::goNomal()
{
    int temp = 0;
    double ans = 0.00;
    step = ui->vEdit->text().toInt();

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
                else
                {
                    pa[0] = 0; pa[1] = 0; pa[2] = 0;
                    pb[0] = 0; pb[1] = 0; pb[2] = 0;
                    pc[0] = 0; pc[1] = 0; pc[2] = 0;
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

