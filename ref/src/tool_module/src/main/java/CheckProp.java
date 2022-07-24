import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;

import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

public class CheckProp {
    Statement mystatement;
    java.sql.Connection myConnection;

    String url = "jdbc:mysql://127.0.0.1:3306/YinLi";
    String user = "root";
    String password = "";
//    String url = "jdbc:mysql://123.56.108.129:3306/YinLi";
//    String user = "syl";
//    String password = "root";

    JSONObject jo = new JSONObject();
    JSONObject jolist = new JSONObject();
    int i=0;//测试后是否有目标程序
    int k=0;
    public CheckProp() throws ClassNotFoundException{
        Class.forName("com.mysql.cj.jdbc.Driver");
    }

    public void Connect()throws SQLException{
        myConnection = DriverManager.getConnection(url, user, password);
        mystatement = myConnection.createStatement();
    }

    public void CheckConnect()throws SQLException{
        if(mystatement != null) {
            mystatement.close();
            mystatement=null;
        }
        if(myConnection !=null) {
            myConnection.close();
            myConnection=null;
        }
    }

    public JSONObject Search(String categories,String category)throws SQLException{
        jo=new JSONObject(true);
        Connect();
        String sql="SELECT * FROM YinLi.prop where "+categories+"='"+category+"'";
        ResultSet rs = mystatement.executeQuery(sql);
        i=0;
        while(rs.next()) {
            i++;
            String fi = rs.getString("id");
            String fm = rs.getString("name");
            String fn = rs.getString("money");
            String fp = rs.getString("properties");
            jo.put("id",fi);
            jo.put("name",fm);
            jo.put("money",fn);
            jo.put("properties", fp);
            System.out.println(jo);
        }
        CheckConnect();
        if(i==0){
            System.out.println("未找到此对象");
            return null;
        }else {
            return jo;
        }
    }

    public JSONObject Search(String categories,String category,String search)throws SQLException{
        jo=new JSONObject();
        jolist=new JSONObject();
        k=0;
        Connect();
        String sql="SELECT * FROM YinLi.prop where "+categories+"='"+category+"'";
        ResultSet rs = mystatement.executeQuery(sql);
        i=0;
        while(rs.next()) {
            i++;
            k++;
            String fi = rs.getString("id");
            String fm = rs.getString("name");
            String fn = rs.getString("money");
            String fp = rs.getString("properties");
            jo.put("id",fi);
            jo.put("name",fm);
            jo.put("money",fn);
            jo.put("properties", fp);

            jolist.put("item_"+k,jo);
            jo=new JSONObject();
            //System.out.println(jo);
        }
        System.out.println(jolist);

        CheckConnect();
        if(i==0){
            System.out.println("未找到此对象");
            return null;
        }else {
            return jolist;
        }
    }



    public boolean NewProp(String name, String money, String properties)throws SQLException{
        Connect();//执行INSERT语句，返回插入的记录数
        String sql="INSERT INTO YinLi.prop(name,money,properties) VALUES ("+"'"+name+"'"+",'"+money+"',"+"'"+properties+"'"+")";
        if( mystatement.executeUpdate(sql) == 1){
            System.out.println("成功添加新道具");
            CheckConnect();
            return true;
        }else{
            System.out.println("添加失败");
            CheckConnect();
            return false;
        }
    }

    public boolean UpdateProp(String id,String name, String money, String properties)throws SQLException{
        Connect();
        String sql="UPDATE YinLi.prop SET name='"+name+"',"+"money="+money+",properties='"+properties+"'where id="+id;
        if( mystatement.executeUpdate(sql) == 1){
            System.out.println("成功修改"+id+"号对象"); //UPDATE，返回修改的记录数
            CheckConnect();
            return true;
        }else{
            System.out.println("未找到此对象");
            CheckConnect();
            return false;
        }
    }

    public void DeleteProp(int id)throws SQLException{
        Connect();
        String sql="DELETE FROM YinLi.prop where id="+id;//DELETE
        if( mystatement.executeUpdate(sql) == 1){
            System.out.println("成功删除");
        }else{
            System.out.println("未找到此对象");
        }
        CheckConnect();
    }

    public boolean Update_User_Prop(String trade,String user_id,String tool_id,String num)throws SQLException{
        Connect();
        String sql="";
        if(trade.equals("minus")) {
            sql = "UPDATE YinLi.user_prop SET num=num-" + num + " where user_id=" + user_id + " and tool_id=" + tool_id;

            if (mystatement.executeUpdate(sql) == 1) {
                System.out.println(user_id + "号玩家成功发货" + num + "件" + tool_id + "号道具");//UPDATE，返回修改的记录数
                return true;
            } else {
                System.out.println(user_id + "号玩家发货错误，未找到库内可发货对象！");
                CheckConnect();
                return false;
            }
        }
        else if (trade.equals("add")){
            sql = "UPDATE YinLi.user_prop SET num=num+" + num + " where user_id=" + user_id + " and tool_id=" + tool_id;

            if (mystatement.executeUpdate(sql) == 1){
                System.out.println(user_id+"号玩家成功收货"+num+"件"+tool_id+"号道具");
                CheckConnect();
                return true;
            }
            else {
                New_User_Prop(user_id,tool_id,num);
                CheckConnect();
                return true;
            }
            }
             return false;
        }

    public JSONObject Search_user_prop(String user_id,String tool_id)throws SQLException{
        Connect();
        String sql="SELECT user_id,tool_id,num FROM YinLi.user_prop where user_id="+user_id+" and tool_id=" + tool_id;
        ResultSet rs = mystatement.executeQuery(sql);

        i=0;
        String f1="";
        String f2="";
        String f3="";

        while(rs.next()) {
            i++;
            f1 = rs.getString("user_id");
            f2 = rs.getString("tool_id");
            f3 = rs.getString("num");
        }
//        JSONObject jsonReturn = new JSONObject();

        JSONObject jsonObject = new JSONObject();
        jsonObject.put("user_id", f1);
        jsonObject.put("tool_id", f2);
        jsonObject.put("num", f3);
//        jsonReturn.put("user",jsonObject);

        CheckConnect();
        if(i==0){
            System.out.println("未找到此对象");
            return null;
        }else {
            return jsonObject;
        }
    }

    public void New_User_Prop(String user_id, String tool_id, String num)throws SQLException{
        Connect();//执行INSERT语句，返回插入的记录数
        String sql="INSERT INTO YinLi.user_prop(user_id,tool_id,num) VALUES ("+"'"+user_id+"'"+","+tool_id+","+"'"+num+"'"+")";
        if( mystatement.executeUpdate(sql) == 1){
            System.out.println(user_id+"号玩家成功获得"+tool_id+"号新道具");
        }else{
            System.out.println("道具收货失败！");
        }
        CheckConnect();
    }

}

