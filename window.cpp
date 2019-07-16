#include "window.h"


Window::Window(QWidget *parent) : QWidget(parent)
{

    std::ifstream input_previousSettings("previousSettings.txt", std::ifstream::in);
    std::string line;
    int i{0},initial_statusOfSensor,initial_sensorType;
    QString initial_pathToFileList,initial_pathToAnalysisDirectory,initial_sensorName,initial_noiseLimit,initial_seedCut,initial_neighbourCut,initial_minimumClusterSize,initial_lowerTimeCut,initial_upperTimeCut,initial_temperature,initial_plotTitle;

    while (std::getline(input_previousSettings,line))
    {
        i++;
        if (i==1) initial_pathToFileList = QString::fromStdString(line);
        if (i==2) initial_pathToAnalysisDirectory = QString::fromStdString(line);
        if (i==3) initial_sensorName = QString::fromStdString(line);
        if (i==4) initial_statusOfSensor = std::stoi(line);
        if (i==5) initial_seedCut = QString::fromStdString(line);
        if (i==6) initial_neighbourCut = QString::fromStdString(line);
        if (i==7) initial_minimumClusterSize = QString::fromStdString(line);
        if (i==8) initial_lowerTimeCut = QString::fromStdString(line);
        if (i==9) initial_upperTimeCut = QString::fromStdString(line);
        if (i==10) initial_temperature = QString::fromStdString(line);
        if (i==11) initial_plotTitle = QString::fromStdString(line);
        if (i==12) initial_sensorType = std::stoi(line);
    }
    input_previousSettings.close();


    QDesktopWidget screenSize;
    QRect mainScreenSize = screenSize.availableGeometry(screenSize.primaryScreen());
    setFixedSize(mainScreenSize.width()*1.1,mainScreenSize.height()*0.80);

    
    
    ClusterTimePicture = new QGraphicsPixmapItem(QPixmap("temp.png"));
   // ClusterTimePicture->setPos()
    
    statusOfSensor = new QComboBox(this);
    int statusOfSensorxStart = 60, statusOfSensoryStart = 60,statusOfSensorxLength = 200, statusOfSensoryLength = 50;
    statusOfSensor->setGeometry(statusOfSensorxStart,statusOfSensoryStart,statusOfSensorxLength,statusOfSensoryLength);
    statusOfSensor->addItem("Unirradiated");
    statusOfSensor->addItem("Irradiated Pre-Anneal");
    statusOfSensor->addItem("Irradiated Post-Anneal");
    statusOfSensor->setCurrentIndex(initial_statusOfSensor);
    label_statusOfSensor = new QLabel(this);
    label_statusOfSensor->setGeometry(statusOfSensorxStart,statusOfSensoryStart-35,statusOfSensorxLength,statusOfSensoryLength);
    label_statusOfSensor->setText("Sensor Status");



     SensorType = new QComboBox(this);
     int SensorTypexStart = 560, SensorTypeyStart = 420,SensorTypexLength = 240, SensorTypeyLength = 50;
     SensorType->setGeometry(SensorTypexStart,SensorTypeyStart,SensorTypexLength,SensorTypeyLength);
     SensorType->addItem("Channels 1-128");
     SensorType->addItem("Channels 129-256");
     SensorType->setCurrentIndex(initial_sensorType);
     label_SensorType = new QLabel(this);
     label_SensorType->setGeometry(SensorTypexStart,SensorTypeyStart-35,SensorTypexLength,SensorTypeyLength);
     label_SensorType->setText("Channel Range");


    input_pathToFileList = new QLineEdit(this);
    int input_pathToFileListxStart = 300, input_pathToFileListyStart = 60,input_pathToFileListxLength = 500, input_pathToFileListyLength = 50;
    input_pathToFileList->setGeometry(input_pathToFileListxStart,input_pathToFileListyStart,input_pathToFileListxLength,input_pathToFileListyLength);
    input_pathToFileList->setText(initial_pathToFileList);
    label_pathToFileList = new QLabel(this);
    label_pathToFileList->setGeometry(input_pathToFileListxStart,input_pathToFileListyStart-35,input_pathToFileListxLength,input_pathToFileListyLength);
    label_pathToFileList->setText("FileList Directory");

    input_pathToAnalysisDirectory = new QLineEdit(this);
    int input_pathToAnalysisDirectoryxStart = 300, input_pathToAnalysisDirectoryyStart = 150,input_pathToAnalysisDirectoryxLength = 500, input_pathToAnalysisDirectoryyLength = 50;
    input_pathToAnalysisDirectory->setGeometry(input_pathToAnalysisDirectoryxStart,input_pathToAnalysisDirectoryyStart,input_pathToAnalysisDirectoryxLength,input_pathToAnalysisDirectoryyLength);
    input_pathToAnalysisDirectory->setText(initial_pathToAnalysisDirectory);
    label_pathToAnalysisDirectory = new QLabel(this);
    label_pathToAnalysisDirectory->setGeometry(input_pathToAnalysisDirectoryxStart,input_pathToAnalysisDirectoryyStart-35,input_pathToAnalysisDirectoryxLength,input_pathToAnalysisDirectoryyLength);
    label_pathToAnalysisDirectory->setText("Analysis Directory");

    input_sensorName = new QLineEdit(this);
    int input_sensorNamexStart = 300, input_sensorNameyStart = 240,input_sensorNamexLength = 500, input_sensorNameyLength = 50;
    input_sensorName->setGeometry(input_sensorNamexStart,input_sensorNameyStart,input_sensorNamexLength,input_sensorNameyLength);
    input_sensorName->setText(initial_sensorName);
    label_sensorName = new QLabel(this);
    label_sensorName->setGeometry(input_sensorNamexStart,input_sensorNameyStart-35,input_sensorNamexLength,input_sensorNameyLength);
    label_sensorName->setText("code+irradiation+date");



    input_seedCut = new QLineEdit(this);
    int input_seedCutxStart = 60, input_seedCutyStart = 330,input_seedCutxLength = 200, input_seedCutyLength = 50;
    input_seedCut->setGeometry(input_seedCutxStart,input_seedCutyStart,input_seedCutxLength,input_seedCutyLength);
    input_seedCut->setText(initial_seedCut);
    label_seedCut = new QLabel(this);
    label_seedCut->setGeometry(input_seedCutxStart,input_seedCutyStart-35,input_seedCutxLength,input_seedCutyLength);
    label_seedCut->setText("Seed Cut: Standard is 3.5");

    input_neighbourCut = new QLineEdit(this);
    int input_neighbourCutxStart = 300, input_neighbourCutyStart = 330,input_neighbourCutxLength = 240, input_neighbourCutyLength = 50;
    input_neighbourCut->setGeometry(input_neighbourCutxStart,input_neighbourCutyStart,input_neighbourCutxLength,input_neighbourCutyLength);
    input_neighbourCut->setText(initial_neighbourCut);
    label_neighbourCut = new QLabel(this);
    label_neighbourCut->setGeometry(input_neighbourCutxStart,input_neighbourCutyStart-35,input_neighbourCutxLength,input_neighbourCutyLength);
    label_neighbourCut->setText("Neighbour Cut: Standard is 1.8");

    input_minimumClusterSize = new QLineEdit(this);
    int input_minimumClusterSizexStart = 560, input_minimumClusterSizeyStart = 330,input_minimumClusterSizexLength = 240, input_minimumClusterSizeyLength = 50;
    input_minimumClusterSize->setGeometry(input_minimumClusterSizexStart,input_minimumClusterSizeyStart,input_minimumClusterSizexLength,input_minimumClusterSizeyLength);
    input_minimumClusterSize->setText(initial_minimumClusterSize);
    label_minimumClusterSize = new QLabel(this);
    label_minimumClusterSize->setGeometry(input_minimumClusterSizexStart,input_minimumClusterSizeyStart-35,input_minimumClusterSizexLength,input_minimumClusterSizeyLength);
    label_minimumClusterSize->setText("Min Clust Size: Standard is 4");

    input_lowerTimeCut = new QLineEdit(this);
    int input_lowerTimeCutxStart = 60, input_lowerTimeCutyStart = 420,input_lowerTimeCutxLength = 200, input_lowerTimeCutyLength = 50;
    input_lowerTimeCut->setGeometry(input_lowerTimeCutxStart,input_lowerTimeCutyStart,input_lowerTimeCutxLength,input_lowerTimeCutyLength);
    input_lowerTimeCut->setText(initial_lowerTimeCut);
    label_lowerTimeCut = new QLabel(this);
    label_lowerTimeCut->setGeometry(input_lowerTimeCutxStart,input_lowerTimeCutyStart-35,input_lowerTimeCutxLength,input_lowerTimeCutyLength);
    label_lowerTimeCut->setText("Min Time Cut: Standard is 15");


    input_upperTimeCut = new QLineEdit(this);
    int input_upperTimeCutxStart = 300, input_upperTimeCutyStart = 420,input_upperTimeCutxLength = 240, input_upperTimeCutyLength = 50;
    input_upperTimeCut->setGeometry(input_upperTimeCutxStart,input_upperTimeCutyStart,input_upperTimeCutxLength,input_upperTimeCutyLength);
    input_upperTimeCut->setText(initial_upperTimeCut);
    label_upperTimeCut = new QLabel(this);
    label_upperTimeCut->setGeometry(input_upperTimeCutxStart,input_upperTimeCutyStart-35,input_upperTimeCutxLength,input_upperTimeCutyLength);
    label_upperTimeCut->setText("Max Time Cut: Standard is 25");

    input_Temperature = new QLineEdit(this);
    int input_TemperaturexStart = 60, input_TemperatureyStart = 580,input_TemperaturexLength = 740, input_TemperatureyLength = 50;
    input_Temperature->setGeometry(input_TemperaturexStart,input_TemperatureyStart,input_TemperaturexLength,input_TemperatureyLength);
    input_Temperature->setText(initial_temperature);
    label_Temperature = new QLabel(this);
    label_Temperature->setGeometry(input_TemperaturexStart,input_TemperatureyStart-35,input_TemperaturexLength,input_TemperatureyLength);
    label_Temperature->setText("Temperature, comma delimited");

    input_plotTitle = new QLineEdit(this);
    int input_plotTitlexStart = 60, input_plotTitleyStart = 660,input_plotTitlexLength = 740, input_plotTitleyLength = 50;
    input_plotTitle->setGeometry(input_plotTitlexStart,input_plotTitleyStart,input_plotTitlexLength,input_plotTitleyLength);
    input_plotTitle->setText(initial_plotTitle);
    label_plotTitle = new QLabel(this);
    label_plotTitle->setGeometry(input_plotTitlexStart,input_plotTitleyStart-35,input_plotTitlexLength,input_plotTitleyLength);
    label_plotTitle->setText("Overlay Plot Title");






    currentFile = new QLineEdit(this);
    int currentFilexStart = 60, currentFileyStart = 500,currentFilexLength = 590, currentFileyLength = 50;
    currentFile->setGeometry(currentFilexStart,currentFileyStart,currentFilexLength,currentFileyLength);
    currentFile->setText("");
    currentFile->setReadOnly(true);
    label_currentFile = new QLabel(this);
    label_currentFile->setGeometry(currentFilexStart,currentFileyStart-35,currentFilexLength,currentFileyLength);
    label_currentFile->setText("Current File");

    currentEvent = new QLineEdit(this);
    int currentEventxStart = 700, currentEventyStart = 500,currentEventxLength = 100, currentEventyLength = 50;
    currentEvent->setGeometry(currentEventxStart,currentEventyStart,currentEventxLength,currentEventyLength);
    currentEvent->setText("");
    currentEvent->setReadOnly(true);
    label_currentEvent = new QLabel(this);
    label_currentEvent->setGeometry(currentEventxStart,currentEventyStart-35,currentEventxLength,currentEventyLength);
    label_currentEvent->setText("Current Event");

    histogram = new QCustomPlot(this);
    int histogramxStart = 830, histogramyStart = 60,histogramxLength = 490, histogramyLength = 490;
    histogram->setGeometry(histogramxStart,histogramyStart,histogramxLength,histogramyLength);
    histogram->xAxis->setLabel("ADC");
    histogram->yAxis->setLabel("Counts");
    histogram->xAxis->setRange(0, 500);
    histogram->yAxis->setRange(0, 1500);
    histogram->addGraph();
    histogram->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    histogram->graph(0)->setBrush(QBrush(QPixmap("./db.jpg")));

    Qhtotal = new QCustomPlot(this);
    int QhtotalxStart = 830, QhtotalyStart = 560,QhtotalxLength = 490, QhtotalyLength = 200;
    Qhtotal->setGeometry(QhtotalxStart,QhtotalyStart,QhtotalxLength,QhtotalyLength);
    Qhtotal->xAxis->setLabel("ADC");
    Qhtotal->yAxis->setLabel("Counts");
    Qhtotal->xAxis->setRange(0, 400);
    Qhtotal->yAxis->setRange(0, 250);
    Qhtotal->addGraph();
    Qhtotal->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    Qhtotal->graph(0)->setPen(QPen(QColor(255, 100, 0)));


    confirmSettings = new QPushButton(this);
    confirmSettings->setGeometry(60,130,200,150);
    confirmSettings->setText("Confirm Settings?");



    //connect(input_pathToFileList, SIGNAL (returnPressed()), QApplication::instance(), SLOT (quit()) );

    //connect(input_pathToFileList, SIGNAL (returnPressed()), test, SLOT (test->setText("BOO"))  );
    //QObject::connect(input_pathToFileList, SIGNAL (returnPressed()), progressBar, SLOT (setValue(50)));

}
