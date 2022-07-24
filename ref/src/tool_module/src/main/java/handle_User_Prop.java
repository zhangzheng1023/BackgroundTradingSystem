import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;

import java.io.UnsupportedEncodingException;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.HashMap;
import java.util.Map;
import java.sql.SQLException;
import java.util.SplittableRandom;


public class handle_User_Prop {
    public String operation;

    public String id;
    public String url;
    public String user_id;

    public String prop_id;
    public String num;

    public String name;
    public String money;
    public String properties;

    public String category;
    public String categories;

    public handle_User_Prop() {
    }
    public void judgeOpertion(String jsonString) throws MalformedURLException {
        Map<String,Object> queryValue =  JSONObject.parseObject(jsonString);
        String operation=(String)queryValue.get("operation");
        this.operation=operation;
    }

    public void handleUserPropInMysql(String jsonString) throws MalformedURLException, ClassNotFoundException, SQLException {
        Map<String,Object> queryValue =  JSONObject.parseObject(jsonString);
        System.out.println(queryValue.keySet());
        System.out.println(queryValue.values());
        String user_id=(String)queryValue.get("user_id");
        String prop_id=(String)queryValue.get("prop_id");
        String num=(String)queryValue.get("num");

        this.user_id=user_id;
        this.prop_id=prop_id;
        this.num=num;
    }

    public void handle_NewProp(String jsonString) throws MalformedURLException{
        Map<String,Object> queryValue =  JSONObject.parseObject(jsonString);
        System.out.println(queryValue.keySet());
        System.out.println(queryValue.values());
        String name=(String)queryValue.get("name");
        String money=(String)queryValue.get("money");
        String properties= (String)queryValue.get("properties");

        this.name=name;
        this.money=money;
        this.properties=properties;
    }

    public void handle_Search(String jsonString) throws MalformedURLException {
        Map<String,Object> queryValue =  JSONObject.parseObject(jsonString);
        System.out.println(queryValue.keySet());
        System.out.println(queryValue.values());
        String categories=(String)queryValue.get("categories");
        String category=(String)queryValue.get("category");

        this.categories=categories;
        this.category=category;

    }


    public void handle_UpdateProp(String jsonString) throws MalformedURLException {
        Map<String,Object> queryValue =  JSONObject.parseObject(jsonString);
        System.out.println(queryValue.keySet());
        System.out.println(queryValue.values());
        String id=(String)queryValue.get("id");
        String name=(String)queryValue.get("name");
        String money=(String)queryValue.get("money");
        String properties=(String)queryValue.get("properties");

        this.id=id;
        this.name=name;
        this.money=money;
        this.properties=properties;
    }
}
