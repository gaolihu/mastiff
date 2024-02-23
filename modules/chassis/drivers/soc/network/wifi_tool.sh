#!/bin/bash
###
 # @Date: 2023-09-12 20:35:13
 # @LastEditors: xianweijing
 # @FilePath: /aventurier_framework/scripts/wifi/wifi_tool.sh
 # @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
###

WPA_CLI_INTERFACE="wlan0"
wpa_cli="wpa_cli"
# 设置要 ping 的目标主机或IP地址
target_host="baidu.com"

# 自动转义函数,WIFI名里不能带单引号 ' ，否则解析失败
auto_escape() {
  escaped=$(echo "$1" | sed 's/[][(){}*.$^?+`:;&-=%!#@|\\]/\\&/g')
  # 转义单引号
  escaped="${escaped//\'/\\\'}"
  # 转义双引号
  escaped="${escaped//\"/\\\"}"
  echo "$escaped"
}

cli() {
    if [[ ! -z "$WPA_CLI_INTERFACE" ]]; then
        ${wpa_cli} -i $WPA_CLI_INTERFACE "$@"
    else
        ${wpa_cli} "$@"
    fi
}

status(){
    cli status
}

status_json(){
    echo "{"
    first=true

    for s in $(status)
    do
        if [ "$first" = true ]; then
            first=false
        else
            echo ","
        fi

        echo "$s" | awk -F= '{printf "  \"%s\":\"%s\"", $1, $2}'
    done

    echo
    echo "}"
}
scan(){
    cli scan
}
scan_results() {
    # 重新扫描一下
    scan >/dev/null 2>&1
    # sync
    # sleep 1
    curr_results=$(cli scan_results  |grep -v "bssid / frequency / signal level / flags / ssid" | sort -r -g -k 3)

    # 将输出转换为 JSON 格式
    json_output="{\"scan_results\": ["

    # 使用 awk 将每行转换为 JSON 格式的条目
    json_output+=$(echo "$curr_results" | awk -v ORS='\n' 'NR>1 && /^[0-9a-fA-F:]+/ {
        if (NR > 2) {
            printf ","
        }
        s_level = $3 + 100
        printf "{\"bssid\":\"%s\",\"frequency\":\"%s\",\"signal_level\":%d,\"ssid\":\"%s\",\"flags\":\"%s\"}", $1, $2, s_level, $5, $6
        printf "\n"
    }')

    # 添加 JSON 结束标记
    json_output+="]}"
    # 输出 JSON 格式的结果
    echo "$json_output"
}
up(){
    ifconfig $WPA_CLI_INTERFACE up
}
down(){
    ifconfig $WPA_CLI_INTERFACE down
}
info(){
    cat /data/cfg/wpa_supplicant.conf
}
ip(){
    status | grep ip_address= | sed 's/ip_address=//'
}
mac(){
    status | grep -oE '^address=[0-9a-fA-F:]+' | sed 's/address=//'
}
ssid(){
    status | grep ssid | grep -v bssid | grep -v grep | sed 's/ssid=//'
}
bssid(){
    status | grep bssid | grep -v grep | sed 's/bssid=//'
}
mode(){
    iw dev $WPA_CLI_INTERFACE info | grep type | awk '{print $2}'
}
dns(){
    cat /etc/resolv.conf | grep -v '#' | awk '{print $2}'
}

calc_ping_time(){
    # 使用 ping 命令尝试连接目标主机，并将输出存储在变量 ping_output 中
    ping_output=$(ping -c 3 "$target_host")

    # 使用 grep 和 awk 提取平均响应时间
    average_response_time=$(echo "$ping_output" | grep -oE 'time=[0-9.]+ ms' | awk -F'=' '{print $2}')

    use_time=0

    # 检查是否成功提取了平均响应时间
    if [ -n "$average_response_time" ]; then
        #echo "平均响应时间为:"
        use_time="$(echo "$ping_output" | grep "min/avg/max" | awk -F'/' '{print $4}')" # ms
    else
        #echo "无法提取平均响应时间"
        use_time=-1
    fi

    echo $use_time
}
check_connection(){
    # 使用 ping 命令尝试连接目标主机，发送3个ICMP回显请求包（ping包）
    use_time=$(calc_ping_time | awk '{printf("%.0f\n", $0)}' )

    # 检查 ping 命令的返回值
    if [ $use_time -ge 0 ]; then
        echo "网络连接正常"
    else
        echo "网络连接失败"
    fi
}

disconnect(){
    #关闭部分程序，防止重复运行
    killall wpa_supplicant   > /dev/null 2>&1

    # 检查是否存在 hostapd 命令
    if command -v hostapd >/dev/null 2>&1; then
        killall hostapd
    else
        echo "hostapd 不存在"
        exit 1
    fi
    # 检查是否存在 dnsmasq 命令
    if command -v dnsmasq >/dev/null 2>&1; then
        killall dnsmasq
    else
        echo "dnsmasq 不存在"
        exit 1
    fi

    up

    sync
    sleep 1 # 等待 killall 结束

    ifconfig $WPA_CLI_INTERFACE 0
}

connect(){
    disconnect

    echo "ctrl_interface=/var/run/wpa_supplicant
ap_scan=1
update_config=1
" > /data/cfg/wpa_supplicant.conf

    wpa_passphrase "$2" "$3"  >> /data/cfg/wpa_supplicant.conf
    sync

    if [ -f "/tmp/wpa.log" ]; then
        rm "/tmp/wpa.log"
    fi

    wpa_supplicant -B -i $WPA_CLI_INTERFACE -t -d -c /data/cfg/wpa_supplicant.conf -f /tmp/wpa.log

    max_count=15 # 最多等待15s, 验证是否能连上
    while [ $max_count  -gt 0 ]
    do
        sync

        if [ "" != "$(cat "/tmp/wpa.log" | grep -i "No suitable network found")" ]; then # 平均5s
            echo "ssid not found"
            break
        fi
        if [ "" != "$(cat "/tmp/wpa.log" | grep -i "pre-shared key may be incorrect")" ]; then # 平均10s
            echo "password incorrect"
            break
        fi
        if [ "" != "$(cat "/tmp/wpa.log" | grep -i "CTRL-EVENT-CONNECTED")" ]; then # 平均6s
            echo "successed"
            break
        fi
        max_count=$((max_count - 1))
        sleep 1
    done

    # ip 地址分配
    if [ -z "$(ps -ef | grep dhcpcd | grep -v grep | awk '{print $1}')" ]; then
        # no dhcp client
        dhcpcd & > /dev/null 2>&1        # 打开dhcp客户端
    else
        dhcpcd $WPA_CLI_INTERFACE
    fi

    # 检查是否已经分配好了ip
    sleep 5 # 分配ip 不会很快，最少5秒
    max_count=10
    while [ $max_count -gt 0 ]
    do
        if [ -n "$(ip)" ]; then
            echo "OK"
            break
        fi
        max_count=$((max_count -1 ))
        sleep 1
    done
}

ap(){
    # 检查热点名称，密码长度
    ap_len=${#2}
    # 热点名称长度是否大于0且小于32
    if [ $ap_len -gt 0 ] && [ $ap_len -lt 32 ]; then
        echo "热点名称的长度大于0且小于32"
    else
        echo "热点名称的长度不符合要求"
        exit 1
    fi
    pwd_len=${#3}
    # 热点密码长度是否大于7且小于64
    if [ $pwd_len -gt 7 ] && [ $pwd_len -lt 64 ]; then
        echo "热点密码的长度大于7且小于64"
    else
        echo "热点密码的长度不符合要求"
        exit 1
    fi

    disconnect

    echo "interface=$WPA_CLI_INTERFACE
driver=nl80211
ssid=$2
hw_mode=g
channel=6
wpa=2
wpa_key_mgmt=WPA-PSK
wpa_passphrase=$3
" > "/etc/hostapd.conf"

    echo "interface=$WPA_CLI_INTERFACE
dhcp-range=192.168.3.2,192.168.3.254,255.255.255.0,24h
" > "/etc/dnsmasq.conf"

    sync

    ifconfig $WPA_CLI_INTERFACE 192.168.3.1
    hostapd  -B "/etc/hostapd.conf"  >/dev/null 2>&1
    dnsmasq
}

reconnect(){
    #关闭部分程序，防止重复运行
    disconnect
    ps -ef | grep  wpa_supplicant

    wpa_supplicant -B -i $WPA_CLI_INTERFACE -c /data/cfg/wpa_supplicant.conf

    if [ -z "$(ps -ef | grep dhcpcd | grep -v grep | awk '{print $1}')" ]; then
        # no dhcp client
        dhcpcd > /dev/null 2>&1        # 重开dhcp客户端
    else
        dhcpcd $WPA_CLI_INTERFACE
    fi

    # 检查是否已经分配好了ip
    sleep 5 # 分配ip 不会很快，最少5秒
    max_count=10
    while [ $max_count -gt 0 ]
    do
        if [ -n "$(ip)" ]; then
            echo "OK"
            break
        fi
        max_count=$((max_count -1 ))
        sleep 1
    done
}

usage(){
    # 定义帮助信息
    help_message="
Usage: /userdata/wifi/wifi_tool.sh [OPTIONS]\n
\n
This is a awesome script that demonstrates how to use wifi tool.\n
\n
Options: \n
  -h, --help                     \t\t\t[Show this help message.] \n
  -v, --version                  \t\t\t[Show the script version.] \n
  status                         \t\t\t[打印wpa状态] \n
  scan                           \t\t\t\t[扫描wifi] \n
  results, scan_results, list    \t[显示扫描到的WIFI列表] \n
  up                             \t\t\t\t[打开wlan0] \n
  down                           \t\t\t\t[关闭wlan0] \n
  info                           \t\t\t\t[wpa conf配置信息] \n
  ip                             \t\t\t\t[ip地址] \n
  mac                            \t\t\t\t[mac地址] \n
  ssid, name                     \t\t\t[连接的wifi名称] \n
  router_mac, bssid              \t\t[连接的路由器mac地址] \n
  mode                           \t\t\t\t[工作模式] \n
  dns                            \t\t\t\t[dns地址] \n
  check_connection, ping         \t[是否能联网(ping通百度)] \n
  calc_ping_time, speed          \t\t[联网速度(ping通百度平均延迟,单位ms)] \n
  connect, conn '名称' '密码'     \t[连接WIFI] \n
  disconnect, diss, break        \t[断开wifi连接] \n
  ap '名称' '密码'                \t\t[开启热点] \n
  reconnect                      \t\t\t[重新连接] \n

"
    echo -e $help_message
}
case $1 in
    "status")
        status
        ;;
    "scan")
        scan
        ;;
    "results" | "scan_results" | "list")
        scan_results
        ;;
    "up")
        up
        ;;
    "down")
        down
        ;;
    "info")
        info
        ;;
    "ip")
        ip
        ;;
    "mac")
        mac
        ;;
    "ssid" | "name")
        ssid
        ;;
    "router_mac" | "bssid")
        bssid
        ;;
    "mode")
        mode
        ;;
    "dns")
        dns
        ;;
    "check_connection" | "ping")
        check_connection
        ;;
    "calc_ping_time" | "speed")
        calc_ping_time
        ;;
    "connect" | "conn")
        connect "$@"
        ;;
    "disconnect" | "dis" | "diss" | "break")
        disconnect
        ;;
    "ap")
        ap "$@"
        ;;
    "reconnect")
        reconnect
        ;;
    "help" | "-h" | "--help")
        usage
        ;;
    "-v" | "--version")
        echo "wifi tool by xianweijing v1.0.0"
        ;;
    *)
    usage
    ;;
esac


