#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_form.h"
#include "client.h"
#include "config.h"
#include <QMenu>
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void on_pushButton_search_clicked();

    void on_treeWidget_devices_doubleClicked(const QModelIndex &index);

    void on_treeWidget_devices_customContextMenuRequested(const QPoint &pos);


    void add_camera(bool checked)
    {
        qDebug()<<"add camera  "<<checked;
        QTreeWidgetItem *tmp_item=new QTreeWidgetItem(QStringList("edit here"));
        p_item_device_root->addChild(tmp_item);
        tmp_item->setFlags(Qt::ItemIsEditable | tmp_item->flags()) ;
        p_item_device_root->setExpanded(true);
    }

    void del_camera(bool checked)
    {
        char buf[2000];
        qDebug()<<"del camera "<<checked;
        int now=p_item_device_current->parent()->indexOfChild(p_item_device_current);
        if(now+1>p_cfg->cfg.camera_amount)//remove camera which didnt submit yet
        {
            p_item_device_root->removeChild(p_item_device_current);
        }else{//remove exsited camera
            int del_index=p_item_device_current->parent()->indexOfChild(p_item_device_current)+1;
            if(selected_camera_index>0){//playing
                if(selected_camera_index==del_index)
                {
                   prt(info,"stop playing %d",del_index);
                  //  window->openGLWidget->stop();//TODO:stop playing
                   selected_camera_index=0;//means no camera playing
                }
            }
            //  int size=cam_manager->get_size();
            int size = p_cfg->cfg.camera_amount;
            if(del_index<=size&&del_index>0)
            {
                //cam_manager->del_camera(del_index);// delete camera local
                p_cfg->del_camera(del_index);
                Protocol::encode_delcam_request(buf,del_index);//encode buffer
                clt->call_server(buf,Protocol::HEAD_LENGTH);//talk to server
            }
            p_item_device_root->removeChild(p_item_device_current);

        }
    }

    void submit_camera(bool checked)
    {
        char buf[2000];
        QString ip=p_item_device_current->text((0));
        p_cfg->append_camera(ip);
        QByteArray setting=p_cfg->get_config();//get new config from local database
        int len=Protocol::encode_addcam_request(buf,setting.length());//encode buffer
        memcpy(buf+Protocol::HEAD_LENGTH,setting.data(),setting.length());
        QByteArray rst=clt->call_server(buf,len);//talk to server
        // itm->setFlags(!Qt::ItemIsEditable | itm_root->flags()) ;
        p_item_device_current->setFlags(  p_item_device_current->flags() & ~Qt::ItemIsEditable) ;
    }

private:
    Ui::Form *window;
    ServerInfoSearcher *searcher;
    Client *clt;
    CameraConfiguration *p_cfg;
    QTreeWidgetItem *p_item_device_root;
    QTreeWidgetItem *p_item_device_current;
    int selected_camera_index;
};

#endif // MAINWINDOW_H
