import com.alibaba.fastjson.JSONObject;

import java.io.*;
import java.net.Socket;
import java.sql.SQLException;
import java.util.Map;


public class RequestExecute extends Thread{
    private Socket socket;
    public RequestExecute(Socket socket){
        this.socket=socket;
    }
    @Override
    public void run() {
        InputStream in=null;
        InputStreamReader reader=null;
        BufferedReader bufferedReader=null;
        OutputStream out=null;
        PrintWriter pw=null;
        try {

            out=socket.getOutputStream();
            pw=new PrintWriter(out);

            in=socket.getInputStream();
            reader=new InputStreamReader(in);
            bufferedReader=new BufferedReader(reader);

            String line=null;
            int rline=-1;
            int lineNum=1;
            String reqPath="";
            String host="";
            String Url="";
            String operation="";
            char[] ch=new char[1024];
            int value=-1;

            value=bufferedReader.read(ch);
            line=new String(ch);
            System.out.println(line);

                if(lineNum==1){
                    String[] infos = line.split(" ");
                    if(infos!=null||infos.length>2){
                        reqPath=infos[1];
                    }
                    else{
                        throw new RuntimeException("请求行解析失败："+line);
                    }
                }else {
                    String[] infos = line.split(": ");
                    if(infos!=null||infos.length==2){
                        if(infos[0].equals("Host"))
                            host=infos[1];
                    }
                }

                String jsonString=line.substring(line.lastIndexOf("{"));
                System.out.println("JSON数据："+jsonString);

            handle_User_Prop judge=new handle_User_Prop();
            judge.judgeOpertion(jsonString);
            operation=judge.operation;

            //1.交易收发货：
            if (operation.equals("minus") ) {
                handle_User_Prop hup = new handle_User_Prop();
                CheckProp cp = new CheckProp();

                hup.handleUserPropInMysql(jsonString);

                JSONObject preSearch = cp.Search_user_prop(hup.user_id, hup.prop_id);
                Map<String, Object> preMap = JSONObject.parseObject(preSearch.toJSONString());
                String preNum = (String) preMap.get("num");
                int num1 = Integer.parseInt(preNum);
                int num2 = Integer.parseInt(hup.num);

                if ((num1 - num2) < 0) {
                    response404(out);
                    System.out.println("玩家道具数量不足！");
                }
                else {
                boolean TorF = cp.Update_User_Prop(operation, hup.user_id, hup.prop_id, hup.num);
                JSONObject JA = cp.Search_user_prop(hup.user_id, hup.prop_id);

                if (TorF) {
                    response200(out, JA, operation);
                    System.out.println("玩家道具数量成功减少");
                } else {
                    response404(out);
                    System.out.println("减少道具操作发生错误！");
                }
              }
            }
            else if(operation.equals("add")){
                handle_User_Prop hup=new handle_User_Prop();
                hup.handleUserPropInMysql(jsonString);

                CheckProp cp=new CheckProp();
                boolean TorF= cp.Update_User_Prop(operation,hup.user_id,hup.prop_id,hup.num);
                JSONObject JA=cp.Search_user_prop(hup.user_id,hup.prop_id);

                if(TorF){
                    response200(out,JA,operation);
                    System.out.println("玩家道具数量成功增加");
                }
                else{
                    response404(out);
                    System.out.println("增加道具操作发生错误！");
                }
            }

            //2.添加新道具：
            else if(operation.equals("newprop")){
                handle_User_Prop hup=new handle_User_Prop();
                hup.handle_NewProp(jsonString);

                CheckProp cp=new CheckProp();
                boolean TorF= cp.NewProp(hup.name,hup.money,hup.properties);
                JSONObject JO=cp.Search("name",hup.name);
                if(TorF){
                    response200(out,JO,"NewProp");
                    System.out.println("返回成功添加道具");
                }
                else{
                    response404(out);

                }

            }

            //3.修改道具信息：
            else if(operation.equals("update")){
                handle_User_Prop hup=new handle_User_Prop();
                hup.handle_UpdateProp(jsonString);

                CheckProp cp=new CheckProp();
                boolean TorF= cp.UpdateProp(hup.id,hup.name,hup.money,hup.properties);
                JSONObject JO=cp.Search("id",hup.id);
                if(TorF){
                    response200(out,JO,"Update");
                    System.out.println("成功修改"+hup.id+"号道具的信息");
                }
                else{
                    response404(out);
                    //System.out.println("交易错误！");
                }

            }

            //4.查询道具：
            else if(operation.equals("search")){
                handle_User_Prop hup=new handle_User_Prop();
                hup.handle_Search(jsonString);

                CheckProp cp=new CheckProp();
                JSONObject JO= cp.Search(hup.categories,hup.category,"Search");
                if(JO!=null){
                    response200(out,JO,"Search");
                    System.out.println("返回所查询道具信息");
                }
                else{
                    response404(out);

                }
            }
            else{
                response404(out);
            }



            //删除道具（空置，题目未作要求）：
//            else if(operation.equals("Delete")){
//                handlel_User_Prop hup=new handlel_User_Prop();
//                hup.handle_NewProp(Url);
//
//                CheckProp cp=new CheckProp();
//                boolean TorF= cp.NewProp(hup.name,hup.money,hup.properties);
//                JSONObject JO=cp.Search("name",hup.name);
//                if(TorF){
//                    response200(out,JO,"Delete");
//                    System.out.println("删除成功");
//                }
//                else{
//                    response404(out);
//
//                }
//            }

            System.out.println("处理请求:http://"+host+reqPath);

        }catch (IOException | ClassNotFoundException | SQLException e){
            e.printStackTrace();
            throw new RuntimeException(e);
        }finally {
            try {
                    if(in!=null)
                        in.close();
                    if(reader!=null)
                        reader.close();
                    if(bufferedReader!=null)
                        bufferedReader.close();
            }catch (IOException ex){
                ex.printStackTrace();
            }
        }
    }
    private void response200(OutputStream out,JSONObject jsonmsg,String reMethod){
        PrintWriter pw=new PrintWriter(out);
        InputStream in=null;
        InputStreamReader reader=null;
        BufferedReader bufferedReader=null;

        try {
               // pw=new PrintWriter(out);
                pw.println("HTTP/1.1 200 OK");
                pw.write("Content-Type:application/json;charset=utf-8");

                pw.println();
                pw.println();
                JSONObject jo = new JSONObject(true);

                if(reMethod.equals("minus")||reMethod.equals("add")) {
                    jo.put("status", "true");
                    jo.put("data",jsonmsg);
                }
                else if(reMethod.equals("NewProp")){
                    jo.put("status:","true");
                    jo.put("data",jsonmsg);
                }
                else if(reMethod.equals("Search")){
                    jo.put("status:","true");
                    jo.put("data",jsonmsg);
                }
                else if(reMethod.equals("Update")){
                    jo.put("status:","true");
                    jo.put("data",jsonmsg);
                }

                pw.println(jo);
                pw.flush();

        }catch (Exception e){
            e.printStackTrace();
        }finally {
            try {
                if(pw!=null)
                    pw.close();
            }catch (Exception ex){
                ex.printStackTrace();
            }
        }
    }

    private void response404(OutputStream out){
        PrintWriter pw=new PrintWriter(out);
        InputStream in=null;
        InputStreamReader reader=null;
        BufferedReader bufferedReader=null;

        try {

            pw.println("HTTP/1.1 404");
            pw.println("Content-Type:text/html;charset=utf-8");

            pw.println();
            pw.println();
            JSONObject jo = new JSONObject();
            jo.put("status","false");
            jo.put("message","ERROR");
            pw.print(jo);
            pw.flush();

        }catch (Exception e){
            e.printStackTrace();
        }finally {
            try {
                if(pw!=null)
                    pw.close();
            }catch (Exception ex){
                ex.printStackTrace();
            }
        }
    }

}
