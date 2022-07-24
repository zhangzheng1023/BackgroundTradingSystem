container_name="trade_module_ins"
image_name="trade_module:1.0"
volumes="-v /root/teams/src/trade_module:/home/"
docker run -itd --name ${container_name}\
        -p 8888:8888 \
		${image_name} 
