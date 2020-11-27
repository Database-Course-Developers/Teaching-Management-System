﻿#include "admin.h"
#include "ui_admin.h"

admin::admin(adm cur_admin,QWidget *parent) :
    QWidget(parent),cur_admin(cur_admin),
    ui(new Ui::admin)
{
    ui->setupUi(this);
    //屏幕最大化
    setWindowState(Qt::WindowMaximized);
    initbox();
}

admin::~admin()
{
    delete ui;
}




//初始化选择框内的选项
void admin::initbox(){
    //初始化学期选择框
    QVector<QString> year=getyear();
    for(QString& s:year){
        ui->cbox_grade_year->addItem(s);
        ui->cbox_grade_year1->addItem(s);

    }
    ui->cbox_grade_year->setCurrentIndex(-1);
    ui->cbox_grade_year1->setCurrentIndex(-1);
    //初始化教室选择框
    QVector<QString> clr=getclassroom();
    for(QString& s:clr){
        ui->cbox_exam_clr->addItem(s);
        ui->cbox_examadd_clr->addItem(s);
    }
    ui->cbox_exam_clr->setCurrentIndex(-1);
    ui->cbox_examadd_clr->setCurrentIndex(-1);

    //初始化学院选择框
    QVector<QString> collegename=getcollegename();
    for(QString&s:collegename){
        ui->cbox_exam_collegename->addItem(s);
    }
    ui->cbox_exam_collegename->setCurrentIndex(-1);

    //日期时间选择框初始化
    QDate date(QDate::currentDate());
    ui->dt_begin_time->setDate(date);
    ui->dt_end_time->setDate(date.addMonths(1));
    ui->dt_examadd_begin->setDate(date);
    ui->dt_examadd_end->setDate(date.addDays(1));
    ui->dt_begin_time->setCalendarPopup(true);
    ui->dt_end_time->setCalendarPopup(true);
    ui->dt_examadd_begin->setCalendarPopup(true);
    ui->dt_examadd_end->setCalendarPopup(true);
    //课务号选择框初始化
    QVector<QString> rcno=getOneItem("select real_course.rcno from real_course,course "
                                     "where real_course.cno=course.cno and is_exam=1 "
                                     "and real_course.rcno not in(select rcno from exam)");
    for(QString&s:rcno){
        ui->cbox_examadd_rcno->addItem(s);
    }
    ui->cbox_examadd_rcno->setCurrentIndex(-1);

    //初始化老师选择框
    QVector<QString> tno=getOneItem("select tno from teacher");
    for(QString&s:tno){
        ui->cbox_examadd_tno1->addItem(s);
        ui->cbox_examadd_tno2->addItem(s);
    }
    ui->cbox_examadd_tno1->setCurrentIndex(-1);
    ui->cbox_examadd_tno2->setCurrentIndex(-1);


}
//-------考务 part -------林嘉欣
QString admin::get_exam_querysql(){
    QString sql="select * from exam_view ";
    QString condition="";
    QString rcno=ui->ld_exam_rcno->text();
    QString cno=ui->ld_exam_cno->text();
    QString tno=ui->ld_exam_tno->text();
    QString tno1=ui->ld_exam_tno1->text();
    QString tno2=ui->ld_exam_tno2->text();
    QString clr=ui->cbox_exam_clr->currentText();
    QString collegename=ui->cbox_exam_collegename->currentText();
    QString begin_time=ui->dt_begin_time->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString end_time=ui->dt_end_time->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    if(rcno.size()){
        condition+=(QString("rcnno='"+rcno+"' and " ));
    }
    if(cno.size()){
        condition+=(QString("cno='"+cno+"' and " ));
    }
    if(tno.size()){
        condition+=(QString("tno='"+tno+"' and " ));
    }
    if(tno1.size()){
        condition+=(QString("tno1='"+tno1+"' and " ));
    }
    if(tno2.size()){
        condition+=(QString("tno2='"+tno2+"' and " ));
    }
    if(clr.size()){
        condition+=(QString("clr='"+clr+"' and " ));
    }
    if(collegename.size()){
        condition+=(QString("collegename='"+tno2+"' and " ));
    }
    if(begin_time.size()){
        condition+=(QString("begin_time>='"+begin_time+"' and " ));
    }
    if(end_time.size()){
        condition+=(QString("end_time<='"+end_time+"' and " ));
    }

    if(condition.size()){
        sql=sql+"where "+ condition.left(condition.size()-4 );//-4去掉最后一个and
    }
    return sql;
}
void admin::on_btn_exam_query_clicked()
{
    ui->table_exam->blockSignals(true);
    QSqlQuery query;
    QString sql=get_exam_querysql();
    if(query.exec(sql)){
        ui->table_exam->clearContents();
        ui->table_exam->setRowCount(0);
        while(query.next()){
            QTableWidgetItem* item[9];
            int rowcount=ui->table_exam->rowCount();
            ui->table_exam->insertRow(rowcount);
            for(int i=0;i<9;i++){
                if(i==7||i==8)
                    item[i]=new QTableWidgetItem(query.value(i).toDateTime().toString("yyyy-MM-dd hh:mm::ss"));
                else
                    item[i]=new QTableWidgetItem(query.value(i).toString());
                if(i<3||i==6){
                    item[i]->setFlags(item[i]->flags()&(~Qt::ItemIsEditable));
                }
                ui->table_exam->setItem(rowcount,i,item[i]);

            }
        }
    }else{
        connectErrorMsg=query.lastError().text();
        QMessageBox::information(nullptr,"数据库查找错误！","数据库查找错误，错误信息:"+connectErrorMsg);
    }
    ui->table_exam->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_exam->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->table_exam->horizontalHeader()->setSectionResizeMode(7,QHeaderView::ResizeToContents);
    ui->table_exam->horizontalHeader()->setSectionResizeMode(8,QHeaderView::ResizeToContents);
    ui->table_exam->blockSignals(false);
}
QString admin::get_exam_addsql(){
    QString rcno=ui->cbox_examadd_rcno->currentText();
    QString tno1=ui->cbox_examadd_tno1->currentText();
    QString tno2=ui->cbox_examadd_tno2->currentText();
    QString clr=ui->cbox_examadd_clr->currentText();
    QString begintime=ui->dt_examadd_begin->dateTime().toString(timeformat);
    QString endtime=ui->dt_examadd_end->dateTime().toString(timeformat);
    if(rcno.size()==0||tno1.size()==0||tno2.size()==0||clr.size()==0
            ||begintime.size()==0||endtime.size()==0){
        return "";
    }
    QString sql=QString::asprintf("insert into  exam values('%s','%s','%s','%s','%s','%s')",
                                  rcno.toStdString().c_str(),begintime.toStdString().c_str(),
                                  endtime.toStdString().c_str(),tno1.toStdString().c_str(),
                                  tno2.toStdString().c_str(),clr.toStdString().c_str());


    return sql;
}
void admin::on_btn_exam_add_clicked()
{
    QString sql=get_exam_addsql();
    if(sql.size()==0){
        QMessageBox::information(nullptr,"错误","添加的信息未填写完全");
    }else{
        QSqlQuery query;
        if(query.exec(sql)){
            ui->cbox_examadd_clr->setCurrentIndex(-1);
            ui->cbox_examadd_rcno->setCurrentIndex(-1);
            ui->cbox_examadd_tno1->setCurrentIndex(-1);
            ui->cbox_examadd_tno2->setCurrentIndex(-1);
            QMessageBox::information(nullptr,"添加成功","添加成功");

        }else{

            connectErrorMsg=query.lastError().text();
            QMessageBox::information(nullptr,"数据库插入错误！","数据库插入错误，错误信息:"+connectErrorMsg);
        }
    }
}

void admin::on_btn_exam_delete_clicked()
{
    int row=ui->table_exam->currentRow();
    if(row!=-1){
        QPushButton *okbtn = new QPushButton(QString("确定"));
        QPushButton *cancelbtn = new QPushButton(QString("取消"));
        QMessageBox *mymsgbox = new QMessageBox;

        mymsgbox->setIcon(QMessageBox::Warning);
        mymsgbox->setWindowTitle(QString("提示"));
        mymsgbox->setText(QString("确定删除？"));
        mymsgbox->addButton(okbtn, QMessageBox::AcceptRole);
        mymsgbox->addButton(cancelbtn, QMessageBox::RejectRole);
        mymsgbox->show();

        mymsgbox->exec();//阻塞等待用户输入
        if (mymsgbox->clickedButton()==okbtn)//点击了OK按钮
        {

            QString deletesql="delete from exam where rcno='"
                    +ui->table_exam->item(row,0)->text()+"'";
            qDebug()<<deletesql;
            QSqlQuery query;
            if(query.exec(deletesql)){
                ui->table_exam->removeRow(row);
                QMessageBox::information(nullptr,"删除成功","删除成功");
            }else{
                connectErrorMsg=query.lastError().text();
                QMessageBox::information(nullptr,"数据库删除错误！","数据库删除错误，错误信息:"+connectErrorMsg);
            }
        }


    }else{
        QMessageBox::information(nullptr,"未选中","未选中删除行,请选择一行进行删除");
    }
}
void admin::on_btn_exam_clear_clicked()
{
    ui->ld_exam_cno->setText("");
    ui->ld_exam_rcno->setText("");
    ui->ld_exam_tno->setText("");
    ui->ld_exam_tno1->setText("");
    ui->ld_exam_tno2->setText("");

}
void admin::on_table_exam_itemChanged(QTableWidgetItem *item)
{
    ui->table_exam->blockSignals(true);
    int row=item->row();
    QString updatesql=QString::asprintf("update exam set teacher1='%s',teacher2='%s',clr='%s',begin_time='%s',end_time='%s'"
                                        " where rcno='%s'",
                                        ui->table_exam->item(row,3)->text().toStdString().c_str(),
                                        ui->table_exam->item(row,4)->text().toStdString().c_str(),
                                        ui->table_exam->item(row,5)->text().toStdString().c_str(),
                                        ui->table_exam->item(row,7)->text().toStdString().c_str(),
                                        ui->table_exam->item(row,8)->text().toStdString().c_str(),
                                        ui->table_exam->item(row,0)->text().toStdString().c_str());


    QSqlQuery query;
    qDebug()<<updatesql;
    if(query.exec(updatesql)){
        QMessageBox::information(nullptr,"修改成功","修改成功");
    }else{
        item->setBackground(Qt::red);
        connectErrorMsg=query.lastError().text();
        QMessageBox::information(nullptr,"数据库修改错误！","数据库修改错误，错误信息:"+connectErrorMsg);

    }
    ui->table_exam->blockSignals(false);
}


//-------成绩 part--------林嘉欣
//根据输入框获取查询成绩sql语句
QString admin::get_grade_querysql(){
    QString sql="select * from grade_view ";
    QString condition="";
    QString sno=ui->ld_grade_sno->text();
    QString cno=ui->ld_grade_cno->text();
    QString cname=ui->ld_grade_cname->text();
    QString sname=ui->ld_grade_sname->text();
    QString year=ui->cbox_grade_year->currentText();
    if(sno.size()){
        condition+=(QString("sno='"+sno+"' and " ));
    }
    if(cno.size()){
        condition+=(QString("cno='"+cno+"' and " ));
    }
    if(cname.size()){
        condition+=(QString("cname='"+cname+"' and " ));
    }
    if(sname.size()){
        condition+=(QString("sname='"+sname+"' and " ));
    }
    if(year.size()){
        condition+=(QString("year='"+year+"' and " ));
    }

    if(condition.size()){
        sql=sql+"where "+ condition.left(condition.size()-4 );//-3去掉最后一个and
    }
    return sql;

}
//根据输入框获取添加成绩sql语句
QString admin::get_grade_addsql(){
    QString sno=ui->ld_grade_sno1->text();
    QString cno=ui->ld_grade_cno1->text();
    QString year=ui->cbox_grade_year1->currentText();
    QString grade=ui->ld_grade_grade1->text();
    if(sno.size()==0||cno.size()==0||year.size()==0||grade.size()==0){
        return "";
    }
    QString sql=QString::asprintf("insert into  grade values('%s','%s','%s',%d)",
                                  sno.toStdString().c_str(),cno.toStdString().c_str(),
                                  year.toStdString().c_str(),grade.toInt());

    return sql;

}
//成绩查询按钮
void admin::on_btn_grade_query_clicked()
{
    ui->table_grade->blockSignals(true);
    QSqlQuery query;
    QString sql=get_grade_querysql();
    if(query.exec(sql)){
        ui->table_grade->clearContents();
        ui->table_grade->setRowCount(0);
        while(query.next()){
            QTableWidgetItem* item[6];
            int rowcount=ui->table_grade->rowCount();
            ui->table_grade->insertRow(rowcount);
            for(int i=0;i<6;i++){
                item[i]=new QTableWidgetItem(query.value(i).toString());
                if(i<5){
                    item[i]->setFlags(item[i]->flags()&(~Qt::ItemIsEditable));
                }
                ui->table_grade->setItem(rowcount,i,item[i]);

            }
        }
    }else{
        connectErrorMsg=query.lastError().text();
        QMessageBox::information(nullptr,"数据库查找错误！","数据库查找错误，错误信息:"+connectErrorMsg);
    }
    ui->table_grade->blockSignals(false);
}
//成绩修改信号
void admin::on_table_grade_itemChanged(QTableWidgetItem *item)
{
    ui->table_grade->blockSignals(true);
    int row=item->row();
    QString updatesql="update grade set grade ="+(item->text())+
            " where sno='"+ui->table_grade->item(row,0)->text()+"' and "+
            "cno='"+ui->table_grade->item(row,2)->text()+"' and "+
            "year='"+ui->table_grade->item(row,4)->text()+"'";

    QSqlQuery query;
    if(query.exec(updatesql)){
        QMessageBox::information(nullptr,"修改成功","修改成功");
    }else{
        item->setBackground(Qt::red);
        connectErrorMsg=query.lastError().text();
        QMessageBox::information(nullptr,"数据库修改错误！","数据库修改错误，错误信息:"+connectErrorMsg);

    }
    ui->table_grade->blockSignals(false);

}
//成绩删除按钮
void admin::on_btn_grade_delete_clicked()
{
    int row=ui->table_grade->currentRow();
    if(row!=-1){
        QPushButton *okbtn = new QPushButton(QString("确定"));
        QPushButton *cancelbtn = new QPushButton(QString("取消"));
        QMessageBox *mymsgbox = new QMessageBox;

        mymsgbox->setIcon(QMessageBox::Warning);
        mymsgbox->setWindowTitle(QString("提示"));
        mymsgbox->setText(QString("确定删除？"));
        mymsgbox->addButton(okbtn, QMessageBox::AcceptRole);
        mymsgbox->addButton(cancelbtn, QMessageBox::RejectRole);
        mymsgbox->show();

        mymsgbox->exec();//阻塞等待用户输入
        if (mymsgbox->clickedButton()==okbtn)//点击了OK按钮
        {

            QString deletesql="delete from grade where sno='"
                    +ui->table_grade->item(row,0)->text()+"' and "+
                    "cno='"+ui->table_grade->item(row,2)->text()+"' and "+
                    "year='"+ui->table_grade->item(row,4)->text()+"'";
            QSqlQuery query;
            if(query.exec(deletesql)){
                ui->table_grade->removeRow(row);
                QMessageBox::information(nullptr,"删除成功","删除成功");
            }else{
                connectErrorMsg=query.lastError().text();
                QMessageBox::information(nullptr,"数据库删除错误！","数据库删除错误，错误信息:"+connectErrorMsg);
            }
        }


    }else{
        QMessageBox::information(nullptr,"未选中","未选中删除行,请选择一行进行删除");
    }
}
//成绩查询重置按钮
void admin::on_btn_grade_clear_clicked()
{
    ui->ld_grade_cno->setText("");
    ui->ld_grade_sno->setText("");
    ui->ld_grade_cname->setText("");
    ui->ld_grade_sname->setText("");
    ui->cbox_grade_year->setCurrentIndex(-1);
}
//成绩添加按钮
void admin::on_bbtn_grade_add_clicked()
{
    QString sql=get_grade_addsql();
    if(sql.size()==0){
        QMessageBox::information(nullptr,"错误","添加的信息未填写完全");
    }else{
        QSqlQuery query;
        if(query.exec(sql)){
            ui->ld_grade_cno1->setText("");
            ui->ld_grade_sno1->setText("");
            ui->ld_grade_grade1->setText("");
            ui->cbox_grade_year1->setCurrentIndex(-1);
            QMessageBox::information(nullptr,"添加成功","添加成功");

        }else{

            connectErrorMsg=query.lastError().text();
            QMessageBox::information(nullptr,"数据库插入错误！","数据库插入错误，错误信息:"+connectErrorMsg);
        }
    }
}






//----------教师管理part----------胡婷婷

//根据输入框获取查询教师信息sql语句
QString admin::get_tea_querysql(){
    QString sql="select * from teacher ";
    QString condition="";
    QString birth_condition="";
    QString tno=ui->ld_tea_no->text();
    QString tname=ui->ld_tea_name->text();
    QString sex=ui->cbox_tea_sex->currentText();
    QString birth_y=ui->cbox_tea_birth_y->currentText();
    QString birth_m=ui->cbox_tea_birth_m->currentText();
    QString birth_d=ui->cbox_tea_birth_d->currentText();
    QString pos=ui->cbox_tea_pos->currentText();
    QString tel=ui->ld_tea_tel->text();
    QString clg=ui->ld_tea_clg->text();
    if(tno.size()){
        condition+=QString("TNO='"+tno+"' and ");
    }
    if(tname.size()){
        condition+=QString("tname='"+tname+"' and ");
    }
    if(sex.size()){
        condition+=QString("sex='"+sex+"' and ");
    }
    //----生日-------
    if(birth_y.size()){
        birth_condition+=QString(birth_y+"%");
    }
    if(birth_m.size()){
        birth_condition+=QString("%-"+birth_m+"-%");
    }
    if(birth_d.size()){
        birth_condition+=QString("%"+birth_d);
    }
    if(birth_condition.size()){
        condition+="birth like '"+birth_condition+"' and ";
    }
    //---------------
    if(pos.size()){
        condition+=QString("position='"+pos+"' and ");
    }
    if(tel.size()){
        condition+=QString("tel='"+tel+"' and ");
    }
    if(clg.size()){
        condition+=QString("CLG='"+clg+"' and ");
    }
    if(condition.size()){
        sql=sql+"where "+condition.left(condition.size()-4);//去掉最后一个and和空格
    }

    return sql;
}

//教师信息查询按钮
void admin::on_btn_tea_query_clicked()
{
    ui->table_tea->blockSignals(true);
    QSqlQuery query;
    QString sql=get_tea_querysql();
    if(query.exec(sql)){
        ui->table_tea->clearContents();
        ui->table_tea->setRowCount(0);
        while(query.next()){
            //执行完query.exec()后query是指向结果集外的
            //第一次执行query.next()使query指向结果集的第一条记录
            //接下来的query.next()使query指向下一条记录
            QTableWidgetItem* item[7];
            int rowCount=ui->table_tea->rowCount();
            ui->table_tea->insertRow(rowCount);
            for(int i=0;i<7;i++){
                item[i]=new QTableWidgetItem(query.value(i).toString());
                item[i]->setFlags((Qt::ItemFlags)0);//单元格设为灰色不可编辑
                ui->table_tea->setItem(rowCount,i,item[i]);
            }
        }
    }
    else{
        //数据库查找错误
    }
    ui->table_tea->blockSignals(false);

}


//----------学生管理part----------胡婷婷
//根据输入框获取查询学生信息sql语句
QString admin::get_stu_querysql(){
    QString sql="select * from student ";
    QString condition="";
    QString birth_condition="";
    QString sno=ui->ld_stu_no->text();
    QString sname=ui->ld_stu_name->text();
    QString sex=ui->cbox_stu_sex->currentText();
    QString hometown=ui->ld_stu_hometown->text();
    QString birth_y=ui->cbox_stu_birth_y->currentText();
    QString birth_m=ui->cbox_stu_birth_m->currentText();
    QString birth_d=ui->cbox_stu_birth_d->currentText();
    QString cls=ui->ld_stu_cls->text();
    if(sno.size()){
        condition+=QString("sno='"+sno+"' and ");
    }
    if(sname.size()){
        condition+=QString("sname='"+sname+"' and ");
    }
    if(sex.size()){
        condition+=QString("sex='"+sex+"' and ");
    }
    if(hometown.size()){
        condition+=QString("hometown='"+hometown+"' and ");
    }
    //----生日-------
    if(birth_y.size()){
        birth_condition+=QString(birth_y+"%");
    }
    if(birth_m.size()){
        birth_condition+=QString("%-"+birth_m+"-%");
    }
    if(birth_d.size()){
        birth_condition+=QString("%"+birth_d);
    }
    if(birth_condition.size()){
        condition+="birth like '"+birth_condition+"' and ";
    }
    //---------------
    if(cls.size()){
        condition+=QString("CLS='"+cls+"' and ");
    }
    if(condition.size()){
        sql=sql+"where "+condition.left(condition.size()-4);//去掉最后一个and和空格
    }

    return sql;
}


//学生信息查询按钮
void admin::on_btn_stu_query_clicked()
{
    ui->table_stu->blockSignals(true);
    QSqlQuery query;
    QString sql=get_stu_querysql();
    if(query.exec(sql)){
        ui->table_tea->clearContents();
        ui->table_tea->setRowCount(0);
        while(query.next()){
            QTableWidgetItem* item[6];
            int rowCount=ui->table_stu->rowCount();
            ui->table_stu->insertRow(rowCount);
            for(int i=0;i<6;i++){
                item[i]=new QTableWidgetItem(query.value(i).toString());
                item[i]->setFlags((Qt::ItemFlags)0);//单元格设为灰色不可编辑
                ui->table_stu->setItem(rowCount,i,item[i]);
            }
        }
    }
    else{
        //数据库查找错误
    }
    ui->table_stu->blockSignals(false);
}
