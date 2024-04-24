#include "mainwindow.h"
#include "ui_mainwindow.h"


#include<QSize>
#include<QIcon>
#include<QContextMenuEvent>
#include<QUrl>
#include<QMessageBox>
#include <QJsonObject>
#include<QJsonDocument>
#include<QJsonArray>
#include"WeatherTool.h"
#include<QPainter>

#define INCREMENT 3//温度每升高或降低一度，y坐标的增量
#define POINT_RADIUS 3//曲线秒点的大小
#define TEXT_OFFSET_X 12
#define TEXT_OFFSET_Y 12

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
    setFixedSize(QSize(width(),height()));
    setWindowIcon(QIcon(":/res/WeaFor.png"));//首先设置窗口无边框，设置固定大小，设置图片背景


    //构建右键菜单
    mExitMenu = new QMenu(this);//创建菜单，并绑定
    mExitAct = new QAction();
    mExitAct->setText(tr("退出"));
    mExitAct->setIcon(QIcon(":/res/close.png"));//设定ACtion事件，并采用图片显示


    mExitMenu->addAction(mExitAct);//将Action绑定到menu中
    //用lamba函数连接退出按钮，触发就退出该应用
    connect(mExitAct, &QAction::triggered, this, [=]() { qApp->exit(0); });//connect函数，触发信号后执行槽函数



    mnetAccessManager = new QNetworkAccessManager(this);
    connect(mnetAccessManager,&QNetworkAccessManager::finished,this,&MainWindow::OnReplied);//使用Qnetworkaccessmanmager来实现网络请求功能，onreplied中同时包含了json文件的解析和界面的更新




    mWeekList << ui->lblWeek0 << ui->lblWeek1 << ui->lblWeek2 << ui->lblWeek3 << ui->lblWeek4 << ui->lblWeek5;//在这里将Qlist中的Qlabel和对应的界面上的Qwidget来进行匹配，方便在updateUI方法中实现对应的数据更新
    mDateList << ui->lblDate0 << ui->lblDate1 << ui->lblDate2 << ui->lblDate3 << ui->lblDate4 << ui->lblDate5;

    mTypeList << ui->lblType0 << ui->lblType1 << ui->lblType2 << ui->lblType3 << ui->lblType4 << ui->lblType5;
    mTypeIconList << ui->lblTypeIcon0 << ui->lblTypeIcon1 << ui->lblTypeIcon2 << ui->lblTypeIcon3 << ui->lblTypeIcon4 << ui->lblTypeIcon5;

    mAqiList << ui->lblQuality0 << ui->lblQuality1 << ui->lblQuality2 << ui->lblQuality3 << ui->lblQuality4 << ui->lblQuality5;

    mFxList << ui->lblFx0 << ui->lblFx1 << ui->lblFx2 << ui->lblFx3 << ui->lblFx4 << ui->lblFx5;
    mFlList << ui->lblFl0 << ui->lblFl1 << ui->lblFl2 << ui->lblFl3 << ui->lblFl4 << ui->lblFl5;

    //将天气类型和对应的图片使用键值对来进行匹配，从而通过解析的数据来实现天气图标的更新
    mTypeMap.insert("暴雪",":/res/type/BaoXue.png");
    mTypeMap.insert("暴雨",":/res/type/BaoYu.png");
    mTypeMap.insert("暴雨到暴雪",":/res/type/BaoYuDaoDaBaoYu.png");
    mTypeMap.insert("大暴雨",":/res/type/DaBaoYu.png");
    mTypeMap.insert("大暴雨到大暴雪",":/res/type/DaBaoYuDaoTeDaBaoYu.png");
    mTypeMap.insert("大到暴雪",":/res/type/DaDaoBaoXue.png");
    mTypeMap.insert("大到暴雨",":/res/type/DaDaoBaoYu.png");
    mTypeMap.insert("大雪",":/res/type/DaXue.png");
    mTypeMap.insert("大雨",":/res/type/DaYu.png");
    mTypeMap.insert("冻雨",":/res/type/DongYu.png");
    mTypeMap.insert("多云",":/res/type/DuoYun.png");
    mTypeMap.insert("浮尘",":/res/type/FuChen.png");
    mTypeMap.insert("雷阵雨",":/res/type/LeiZhenYu.png");
    mTypeMap.insert("雷阵雨伴有冰雹",":/res/type/LeiZhenYuBanYouBingBao.png");
    mTypeMap.insert("霾",":/res/type/Mai.png");
    mTypeMap.insert("强沙尘暴",":/res/type/QiangShaChenBao.png");
    mTypeMap.insert("晴",":/res/type/Qing.png");
    mTypeMap.insert("沙尘暴",":/res/type/ShaChenBao.png");
    mTypeMap.insert("特大暴雨",":/res/type/TeDaBaoYu.png");
    mTypeMap.insert("雾",":/res/type/Wu.png");
    mTypeMap.insert("小到中雨",":/res/type/XiaoDaoZhongYu.png");
    mTypeMap.insert("小到中雪",":/res/type/XiaoDaoZhongXue.png");
    mTypeMap.insert("小雪",":/res/type/XiaoXue.png");
    mTypeMap.insert("小雨",":/res/type/XiaoYu.png");
    mTypeMap.insert("雪",":/res/type/Xue.png");
    mTypeMap.insert("扬沙",":/res/type/YangSha.png");
    mTypeMap.insert("阴",":/res/type/Yin.png");
    mTypeMap.insert("雨",":/res/type/Yu.png");
    mTypeMap.insert("雨夹雪",":/res/type/YuJiaXue.png");
    mTypeMap.insert("阵雨",":/res/type/ZhenYu.png");
    mTypeMap.insert("阵雪",":/res/type/ZhenXue.png");
    mTypeMap.insert("中雨",":/res/type/ZhongYu.png");
    mTypeMap.insert("中雪",":/res/type/ZhongXue.png");


//    getWeatherInfo("101280101");
    getWeatherInfo("重庆");

    //给标签添加事件过滤器
    ui->lblHighCurve->installEventFilter(this);
    ui->lblLowCurve->installEventFilter(this);

    for(int i = 0 ;i<6;i++)
    {
        qDebug()<<"低温"<<mDay[i].low;
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnReplied(QNetworkReply *reply)//将Reply放在私有方法中
{
    qDebug()<<"onReplied successful";
    int status_code=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();//通过QNetworkRequest::HttpStatusCodeAttribute得到请求的状态行：200 443一类的
    qDebug()<<status_code;
    if(reply->error()!=QNetworkReply::NoError||status_code!=200)
    {
        qDebug() << reply->errorString().toLatin1().data();
        QMessageBox::warning(this,"天气","请求数据失败");//请求错误或者没得到200响应成功的状态码弹出messagebox错误框

    }
    else{
        //响应成功就用bytearray去接受浏览器返回的资源内容json文件
        QByteArray biteArray=reply->readAll();
        qDebug()<<""<<biteArray.data();
        parseJson(biteArray);//使用自定义的parsejson方法去解析；浏览器返回的Json信息

    }
    reply->deleteLater();//reply是一个指针将其随后销毁
}



//实现界面跟随鼠标移动功能，主要是重写鼠标事件，mousepressevent和mousemoveevent

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    mExitMenu->exec(QCursor::pos());
    event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    moffset = event->globalPos()-this->pos();//偏离计算距离
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos()-moffset);//根据偏离距离，使得窗口偏离与鼠标一样的量，从而实现跟随鼠标移动
}
//实现界面跟随鼠标移动功能，主要是重写鼠标事件，mousepressevent和mousemoveevent




void MainWindow::getWeatherInfo(QString cityname)//写一个请求的方法，不可能每个人都记得主citycode，因此还要采用键值对的形式，将城市与citycode进行对应，通过城市名去索引citycode，然后传到这个方法中
{
    QString cityCode = WeatherTool::getCityCode(cityname);
    if(cityCode.isEmpty())
    {
        QMessageBox::warning(this,"天气","请检查输入是否正确",QMessageBox::Ok);
        return;
    }
    QUrl url("http://t.weather.itboy.net/api/weather/city/"+cityCode);
    mnetAccessManager->get(QNetworkRequest(url));
}


//Json文件的解析是占最多的点
void MainWindow::parseJson(QByteArray &byteArray)
{
    QJsonParseError err;
    QJsonDocument doc= QJsonDocument::fromJson(byteArray,&err);//首先将json文件转化为jsondocument文件
    if(err.error!=QJsonParseError::NoError)
    {
        return;
    }
    QJsonObject rootobj = doc.object();//将Qjsondocument转化为object对象
    qDebug()<<rootobj.value("message").toString();
    mToday.date = rootobj.value("date").toString();
    mToday.city = rootobj.value("cityInfo").toObject().value("city").toString()//这里需要根据Json文件中的格式进行解析
;

    QJsonObject objData=rootobj.value("data").toObject();
    QJsonObject objyesterday = objData.value("yesterday").toObject();//这里解析昨天的数据，所以需要分开处理，分成了Today和mday来进行处理，其中mday【0】表示昨天
    mDay[0].date=objyesterday.value("ymd").toString();
    mDay[0].week=objyesterday.value("week").toString();
    mDay[0].type=objyesterday.value("type").toString();

    QString s;
    s = objyesterday.value("high").toString().split(" ").at(1);//对最高温度和最低温度的字符串进行分割，并转化为int类型
    s=s.left(s.length()-1);
    mDay[0].high = s.toInt();


    s = objyesterday.value("low").toString().split(" ").at(1);
    s=s.left(s.length()-1);
    mDay[0].low = s.toInt();


    mDay[0].fl=objyesterday.value("fx").toString();
    mDay[0].fx=objyesterday.value("fx").toString();

    mDay[0].aqi=objyesterday.value("aqi").toDouble();


    QJsonArray forecastArr = objData.value("forecast").toArray();//通过一个循环去批量解析Json文件中除了今天以外的所有数据
    for(int i =0 ;i<5;i++)
    {
        QJsonObject objForecast= forecastArr[i].toObject();

        mDay[i+1].week=objForecast.value("week").toString();
        mDay[i+1].date=objForecast.value("ymd").toString();
        mDay[i+1].type=objForecast.value("type").toString();

        QString s;
        s = objForecast.value("high").toString().split(" ").at(1);
        s=s.left(s.length()-1);
        mDay[i+1].high = s.toInt();

        s = objForecast.value("low").toString().split(" ").at(1);
        s=s.left(s.length()-1);
        mDay[i+1].low = s.toInt();
//        qDebug()<<"low"<<mDay[i+1].low;
        mDay[i+1].fl=objForecast.value("fx").toString();
        mDay[i+1].fx=objForecast.value("fx").toString();

        mDay[i+1].aqi=objForecast.value("aqi").toDouble();
    }

    //解析今天的数据
    mToday.ganmao=objData.value("ganmao").toString();
    mToday.wendu=objData.value("wendu").toInt();
    mToday.shidu=objData.value("shidu").toString();
    mToday.pm25=objData.value("pm25").toInt();
    mToday.quality=objData.value("quality").toString();


    mToday.type = mDay[1].type;
    mToday.fx = mDay[1].fx;
    mToday.fl = mDay[1].fl;
    mToday.high = mDay[1].high;
    mToday.low = mDay[1].low;


    //在对所有的数据解析完之后通过定义的updateUI方法来更新界面
    updateUI();

    ui->lblHighCurve->update();
    ui->lblLowCurve->update();


}

void MainWindow::updateUI()
{
    ui->lblDate->setText(QDateTime::fromString(mToday.date,"yyyyMMdd").toString("yyyy/MM/dd")+" "+mDay[1].week);//将解析后的数据更新到对应的界面上的Qwidget
    ui->lblCity->setText(mToday.city);

    ui->lblTemp->setText(QString::number(mToday.wendu));
    ui->lblTemp->setText(mToday.type);
    ui->lblLowHigh->setText(QString::number(mToday.low)+"~"+QString::number(mToday.high)+"C");
    ui->lblGanMao->setText(mToday.ganmao);
    ui->lblWindFx->setText(mToday.fx);
    ui->lblWindFl->setText(mToday.fl);
    ui->lblPM25->setText(QString::number(mToday.pm25));
    ui->lblShiDu->setText(mToday.shidu);
    ui->lblQuality->setText(mToday.quality);



    for(int i = 0;i<6;i++)
    {
        mWeekList[i]->setText("星期" + mDay[i].week.right(1));
        ui->lblWeek0->setText("昨天");
        ui->lblWeek1->setText("今天");
        ui->lblWeek2->setText("明天");
        QStringList ymdList = mDay[i].date.split("-");
        mDateList[i]->setText(ymdList[1] + "/" + ymdList[2]);

        //更新天气类型
        mTypeList[i]->setText(mDay[i].type);
        mTypeIconList[i]->setPixmap(mTypeMap[mDay[i].type]);

        //更新空气质量
        if(mDay[i].aqi >0 && mDay[i].aqi <= 50){
            mAqiList[i]->setText("优");
            mAqiList[i]->setStyleSheet("background-color: rgb(139,195,74);");
        }else if(mDay[i].aqi > 50 && mDay[i].aqi <= 100){
            mAqiList[i]->setText("良");
            mAqiList[i]->setStyleSheet("background-color: rgb(255,170,0);");
        }else if(mDay[i].aqi > 100 && mDay[i].aqi <= 150){
            mAqiList[i]->setText("轻度");
            mAqiList[i]->setStyleSheet("background-color: rgb(255,87,97);");
        }else if(mDay[i].aqi > 150 && mDay[i].aqi <= 200){
            mAqiList[i]->setText("中度");
            mAqiList[i]->setStyleSheet("background-color: rgb(255,17,27);");
        }else if(mDay[i].aqi > 150 && mDay[i].aqi <= 200){
            mAqiList[i]->setText("重度");
            mAqiList[i]->setStyleSheet("background-color: rgb(170,0,0);");
        }else{
            mAqiList[i]->setText("严重");
            mAqiList[i]->setStyleSheet("background-color: rgb(110,0,0);");
        }

        mFxList[i]->setText(mDay[i].fx);
        mFlList[i]->setText(mDay[i].fl);

    }

}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->lblHighCurve && event->type()==QEvent::Paint)
    {
        paintHighCurve();
    }
    if(watched == ui->lblLowCurve && event->type()==QEvent::Paint)
    {
        paintLowCurve();
    }

    return QWidget::eventFilter(watched,event);
}

void MainWindow::paintHighCurve()
{
    QPainter painter(ui->lblHighCurve);
    painter.setRenderHint(QPainter::Antialiasing,true);
    //抗锯齿

    //1.获取x坐标
    int pointx[6] = {0};
    for(int i = 0 ;i<6 ; i++)
    {
        pointx[i]=mWeekList[i]->pos().x()+mWeekList[i]->width()/2;
    }

    //2.获取y坐标
    int tempSum = 0;
    int tempAverage = 0;
    for(int i=0;i<6;i++)
    {
        tempSum +=mDay[i].high;
//        qDebug()<<"test——low"<<mDay[i].high;
    }
    tempAverage = tempSum/6;
    //计算y坐标
    int pointy[6]={0};
    int yCenter = ui->lblHighCurve->height()/2;
    for(int i = 0;i<6;i++)
    {
        pointy[i] = yCenter-((mDay[i].high-tempAverage) * INCREMENT);
    }


    //3.开始绘制

    //初始化画笔
    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(QColor(122,232,0));

    painter.setPen(pen);
    painter.setBrush(QColor(122,232,0));//设置内部填充

    //画点写文本
    for (int i = 0; i<6 ;i++)
    {
        //显示点
        painter.drawEllipse(QPoint(pointx[i],pointy[i]),
                            POINT_RADIUS,POINT_RADIUS);

        //显示具体温度的文本

        painter.drawText(pointx[i]-TEXT_OFFSET_X,pointy[i]-TEXT_OFFSET_Y,QString::number(mDay[i].high)+"*");


    }
    //绘制曲线
    for(int i = 0; i<5; i++)
    {
        if(i==0)
        {
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
        }
        else{
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(pointx[i],pointy[i],pointx[i+1],pointy[i+1]);
    }


}

void MainWindow::paintLowCurve()
{
    QPainter painter(ui->lblLowCurve);
    painter.setRenderHint(QPainter::Antialiasing,true);
    //抗锯齿

    //1.获取x坐标
    int pointx[6] = {0};
    for(int i = 0 ;i<6 ; i++)
    {
        pointx[i]=mWeekList[i]->pos().x()+mWeekList[i]->width()/2;
    }

    //2.获取y坐标
    int tempSum = 0;
    int tempAverage = 0;
    for(int i=0;i<6;i++)
    {
        tempSum +=mDay[i].low;
//        qDebug()<<"test——low"<<mDay[i].low;
    }
    tempAverage = tempSum/6;
    //计算y坐标
    int pointy[6]={0};
    int yCenter = ui->lblLowCurve->height()/2;
    for(int i = 0;i<6;i++)
    {
        pointy[i] = yCenter-((mDay[i].low-tempAverage) * INCREMENT);
    }


    //3.开始绘制

    //初始化画笔
    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(QColor(255,255,255));

    painter.setPen(pen);
    painter.setBrush(QColor(255,255,255));//设置内部填充

    //画点写文本
    for (int i = 0; i<6 ;i++)
    {
        //显示点
        painter.drawEllipse(QPoint(pointx[i],pointy[i]),
                            POINT_RADIUS,POINT_RADIUS);

        //显示具体温度的文本

        painter.drawText(pointx[i]-TEXT_OFFSET_X,pointy[i]-TEXT_OFFSET_Y,QString::number(mDay[i].low)+"^");


    }
    //绘制曲线
    for(int i = 0; i<5; i++)
    {
        if(i==0)
        {
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
        }
        else{
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(pointx[i],pointy[i],pointx[i+1],pointy[i+1]);
    }


}



void MainWindow::on_btnSearch_clicked()
{
    QString cityName = ui->leCity->text();
    getWeatherInfo(cityName);

}

