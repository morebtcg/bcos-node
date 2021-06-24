#!/bin/bash
dirpath="$(cd "$(dirname "$0")" && pwd)"
cd "${dirpath}"
listen_ip="0.0.0.0"
listen_port=30300
node_count=4
fisco_bcos_exec=""
output_dir="${dirpath}/nodes"
binary_name="mini-consensus"

# for cert generation
ca_cert_dir="${dirpath}"
output_dir="${dirpath}"
cert_conf='cert.cnf'
sm_cert_conf='sm_cert.cnf'
days=36500
rsa_key_length=2048
sm_mode='false'
macOS=""
x86_64_arch="true"
sm2_params="sm_sm2.param"
cdn_link_header="https://osp-1257653870.cos.ap-guangzhou.myqcloud.com/FISCO-BCOS"
TASSL_CMD="${HOME}"/.fisco/tassl
nodeid_list=""
nodes_json_file_name="nodes.json"

LOG_WARN() {
    local content=${1}
    echo -e "\033[31m[ERROR] ${content}\033[0m"
}

LOG_INFO() {
    local content=${1}
    echo -e "\033[32m[INFO] ${content}\033[0m"
}

LOG_FALT() {
    local content=${1}
    echo -e "\033[31m[FALT] ${content}\033[0m"
    exit 1
}

dir_must_exists() {
    if [ ! -d "$1" ]; then
        LOG_FALT "$1 DIR does not exist, please check!"
    fi
}

file_must_not_exists() {
    if [ -f "$1" ]; then
        LOG_FALT "$1 file already exist, please check!"
    fi
}

file_must_exists() {
    if [ ! -f "$1" ]; then
        LOG_FALT "$1 file does not exist, please check!"
    fi
}

check_env() {
    [ ! -z "$(openssl version | grep 1.0.2)" ] || [ ! -z "$(openssl version | grep 1.1)" ] || [ ! -z "$(openssl version | grep reSSL)" ] || {
        #echo "download openssl from https://www.openssl.org."
        LOG_FALT "Use \"openssl version\" command to check."
    }
    if [ ! -z "$(openssl version | grep reSSL)" ]; then
        export PATH="/usr/local/opt/openssl/bin:$PATH"
    fi
    if [ "$(uname)" == "Darwin" ]; then
        macOS="macOS"
    fi
    if [ "$(uname -m)" != "x86_64" ]; then
        x86_64_arch="false"
    fi
}

check_and_install_tassl() {
    if [[ "${sm_mode}" == "true" ]]; then
        if [ ! -f "${TASSL_CMD}" ]; then
            # TODO: add tassl v1.1 version binary exec
            local tassl_link_perfix="${cdn_link_header}/FISCO-BCOS/tools/tassl-1.0.2"
            LOG_INFO "Downloading tassl binary from ${tassl_link_perfix}..."
            if [[ -n "${macOS}" ]]; then
                curl -#LO "${tassl_link_perfix}/tassl_mac.tar.gz"
                mv tassl_mac.tar.gz tassl.tar.gz
            else
                if [[ "$(uname -p)" == "aarch64" ]]; then
                    curl -#LO "${tassl_link_perfix}/tassl-aarch64.tar.gz"
                    mv tassl-aarch64.tar.gz tassl.tar.gz
                elif [[ "$(uname -p)" == "x86_64" ]]; then
                    curl -#LO "${tassl_link_perfix}/tassl.tar.gz"
                else
                    LOG_ERROR "Unsupported platform: $(uname -p)"
                    exit 1
                fi
            fi
            tar zxvf tassl.tar.gz && rm tassl.tar.gz
            chmod u+x tassl
            mkdir -p "${HOME}"/.fisco
            mv tassl "${HOME}"/.fisco/tassl
        fi
    fi
}

check_name() {
    local name="$1"
    local value="$2"
    [[ "$value" =~ ^[a-zA-Z0-9._-]+$ ]] || {
        LOG_FALT "$name name [$value] invalid, it should match regex: ^[a-zA-Z0-9._-]+\$"
    }
}

generate_sm_sm2_param() {
    local output=$1
    cat <<EOF >"${output}"
-----BEGIN EC PARAMETERS-----
BggqgRzPVQGCLQ==
-----END EC PARAMETERS-----

EOF
}

generate_sm_cert_conf() {
    local output=$1
    cat <<EOF >"${output}"
HOME			= .
RANDFILE		= $ENV::HOME/.rnd
oid_section		= new_oids

[ new_oids ]
tsa_policy1 = 1.2.3.4.1
tsa_policy2 = 1.2.3.4.5.6
tsa_policy3 = 1.2.3.4.5.7

####################################################################
[ ca ]
default_ca	= CA_default		# The default ca section

####################################################################
[ CA_default ]

dir		= ./demoCA		# Where everything is kept
certs		= $dir/certs		# Where the issued certs are kept
crl_dir		= $dir/crl		# Where the issued crl are kept
database	= $dir/index.txt	# database index file.
#unique_subject	= no			# Set to 'no' to allow creation of
					# several ctificates with same subject.
new_certs_dir	= $dir/newcerts		# default place for new certs.

certificate	= $dir/cacert.pem 	# The CA certificate
serial		= $dir/serial 		# The current serial number
crlnumber	= $dir/crlnumber	# the current crl number
					# must be commented out to leave a V1 CRL
crl		= $dir/crl.pem 		# The current CRL
private_key	= $dir/private/cakey.pem # The private key
RANDFILE	= $dir/private/.rand	# private random number file

x509_extensions	= usr_cert		# The extensions to add to the cert

name_opt 	= ca_default		# Subject Name options
cert_opt 	= ca_default		# Certificate field options

default_days	= 365			# how long to certify for
default_crl_days= 30			# how long before next CRL
default_md	= default		# use public key default MD
preserve	= no			# keep passed DN ordering

policy		= policy_match

[ policy_match ]
countryName		= match
stateOrProvinceName	= match
organizationName	= match
organizationalUnitName	= optional
commonName		= supplied
emailAddress		= optional

[ policy_anything ]
countryName		= optional
stateOrProvinceName	= optional
localityName		= optional
organizationName	= optional
organizationalUnitName	= optional
commonName		= supplied
emailAddress		= optional

####################################################################
[ req ]
default_bits		= 2048
default_md		= sm3
default_keyfile 	= privkey.pem
distinguished_name	= req_distinguished_name
x509_extensions	= v3_ca	# The extensions to add to the self signed cert

string_mask = utf8only

# req_extensions = v3_req # The extensions to add to a certificate request

[ req_distinguished_name ]
countryName = CN
countryName_default = CN
stateOrProvinceName = State or Province Name (full name)
stateOrProvinceName_default =GuangDong
localityName = Locality Name (eg, city)
localityName_default = ShenZhen
organizationalUnitName = Organizational Unit Name (eg, section)
organizationalUnitName_default = fisco
commonName =  Organizational  commonName (eg, fisco)
commonName_default =  fisco
commonName_max = 64

[ usr_cert ]
basicConstraints=CA:FALSE
nsComment			= "OpenSSL Generated Certificate"

subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid,issuer

[ v3_req ]

# Extensions to add to a certificate request

basicConstraints = CA:FALSE
keyUsage = nonRepudiation, digitalSignature

[ v3enc_req ]

# Extensions to add to a certificate request
basicConstraints = CA:FALSE
keyUsage = keyAgreement, keyEncipherment, dataEncipherment

[ v3_agency_root ]
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid:always,issuer
basicConstraints = CA:true
keyUsage = cRLSign, keyCertSign

[ v3_ca ]
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid:always,issuer
basicConstraints = CA:true
keyUsage = cRLSign, keyCertSign

EOF
}

generate_cert_conf() {
    local output=$1
    cat <<EOF >"${output}"
[ca]
default_ca=default_ca
[default_ca]
default_days = 3650
default_md = sha256

[req]
distinguished_name = req_distinguished_name
req_extensions = v3_req
[req_distinguished_name]
countryName = CN
countryName_default = CN
stateOrProvinceName = State or Province Name (full name)
stateOrProvinceName_default =GuangDong
localityName = Locality Name (eg, city)
localityName_default = ShenZhen
organizationalUnitName = Organizational Unit Name (eg, section)
organizationalUnitName_default = FISCO-BCOS
commonName =  Organizational  commonName (eg, FISCO-BCOS)
commonName_default = FISCO-BCOS
commonName_max = 64

[ v3_req ]
basicConstraints = CA:FALSE
keyUsage = nonRepudiation, digitalSignature, keyEncipherment

[ v4_req ]
basicConstraints = CA:TRUE

EOF
}

gen_chain_cert() {

    if [ ! -f "${cert_conf}" ]; then
        generate_cert_conf 'cert.cnf'
    else
        cp "${cert_conf}" .
    fi

    local chaindir="${1}"

    file_must_not_exists "${chaindir}"/ca.key
    file_must_not_exists "${chaindir}"/ca.crt
    file_must_exists 'cert.cnf'

    mkdir -p "$chaindir"
    dir_must_exists "$chaindir"

    openssl genrsa -out "${chaindir}"/ca.key "${rsa_key_length}"
    openssl req -new -x509 -days "${days}" -subj "/CN=FISCO-BCOS/O=FISCO-BCOS/OU=chain" -key "${chaindir}"/ca.key -out "${chaindir}"/ca.crt
    cp "cert.cnf" "${chaindir}"

    LOG_INFO "Build ca cert successfully!"
}

gen_rsa_node_cert() {
    local capath="${1}"
    local ndpath="${2}"
    local node="${3}"

    file_must_exists "$capath/ca.key"
    file_must_exists "$capath/ca.crt"
    check_name node "$node"

    file_must_not_exists "$ndpath"/ssl.key
    file_must_not_exists "$ndpath"/ssl.crt

    mkdir -p "${ndpath}"
    dir_must_exists "${ndpath}"

    openssl genrsa -out "${ndpath}"/ssl.key "${rsa_key_length}"
    openssl req -new -sha256 -subj "/CN=FISCO-BCOS/O=fisco-bcos/OU=agency" -key "$ndpath"/ssl.key -config "$capath"/cert.cnf -out "$ndpath"/ssl.csr
    openssl x509 -req -days "${days}" -sha256 -CA "${capath}"/ca.crt -CAkey "$capath"/ca.key -CAcreateserial \
        -in "$ndpath"/ssl.csr -out "$ndpath"/ssl.crt -extensions v4_req -extfile "$capath"/cert.cnf

    openssl pkcs8 -topk8 -in "$ndpath"/ssl.key -out "$ndpath"/pkcs8_node.key -nocrypt
    cp "$capath"/ca.crt "$capath"/cert.cnf "$ndpath"/

    rm -f "$ndpath"/ssl.csr
    rm -f "$ndpath"/ssl.key

    mv "$ndpath"/pkcs8_node.key "$ndpath"/ssl.key

    LOG_INFO "Build ${node} cert successful!"
}

gen_sm_chain_cert() {
    local chaindir="${1}"
    name=$(basename "$chaindir")
    check_name chain "$name"

    if [ ! -f "${sm_cert_conf}" ]; then
        generate_sm_cert_conf 'sm_cert.cnf'
    else
        cp -f "${sm_cert_conf}" .
    fi

    generate_sm_sm2_param "${sm2_params}"

    mkdir -p "$chaindir"
    dir_must_exists "$chaindir"

    "$TASSL_CMD" genpkey -paramfile "${sm2_params}" -out "$chaindir/sm_ca.key"
    "$TASSL_CMD" req -config sm_cert.cnf -x509 -days "${days}" -subj "/CN=FISCO-BCOS/O=FISCO-BCOS/OU=chain" -key "$chaindir/sm_ca.key" -extensions v3_ca -out "$chaindir/sm_ca.crt"
    cp "${sm_cert_conf}" "${chaindir}"
    cp "${sm2_params}" "${chaindir}"
}

gen_sm_node_cert_with_ext() {
    local capath="$1"
    local certpath="$2"
    local name="$3"
    local type="$4"
    local extensions="$5"

    file_must_exists "$capath/sm_ca.key"
    file_must_exists "$capath/sm_ca.crt"

    file_must_not_exists "$ndpath/sm_${type}.crt"
    file_must_not_exists "$ndpath/sm_${type}.key"

    "$TASSL_CMD" genpkey -paramfile "$capath/${sm2_params}" -out "$certpath/sm_${type}.key"
    "$TASSL_CMD" req -new -subj "/CN=$name/O=fisco-bcos/OU=${type}" -key "$certpath/sm_${type}.key" -config "$capath/sm_cert.cnf" -out "$certpath/sm_${type}.csr"

    echo "not use $(basename "$capath") to sign $(basename $certpath) ${type}" >>"${logfile}"
    "$TASSL_CMD" x509 -sm3 -req -CA "$capath/sm_ca.crt" -CAkey "$capath/sm_ca.key" -days "${days}" -CAcreateserial -in "$certpath/sm_${type}.csr" -out "$certpath/sm_${type}.crt" -extfile "$capath/sm_cert.cnf" -extensions "$extensions"

    rm -f "$certpath/sm_${type}.csr"
}

gen_sm_node_cert() {
    local capath="${1}"
    local ndpath="${2}"

    file_must_exists "$capath/sm_ca.key"
    file_must_exists "$capath/sm_ca.crt"

    mkdir -p "$ndpath"
    dir_must_exists "$ndpath"
    local node=$(basename "$ndpath")
    check_name node "$node"

    gen_sm_node_cert_with_ext "$capath" "$ndpath" "$node" node v3_req
    cat "${capath}/sm_ca.crt" >>"$ndpath/sm_ssl.crt"
    gen_sm_node_cert_with_ext "$capath" "$ndpath" "$node" ennode v3enc_req
    #nodeid is pubkey
    $TASSL_CMD ec -in "$ndpath/sm_ssl.key" -text -noout 2>/dev/null | sed -n '7,11p' | sed 's/://g' | tr "\n" " " | sed 's/ //g' | awk '{print substr($0,3);}' | cat >"$ndpath/sm_ssl.nodeid"

    cp "$capath/sm_ca.crt" "$ndpath"
}

help() {
    cat <<EOF
Usage:
    -c <node count>                     [Optional] install node count, default 4
    -d <output dir>                     [Optional] output directory, default ./nodes
    -e <mini-consensus exec>                   [Required] mini-consensus binay exec
    -p <listenPort>                     [Optional] start listen port, default 30300
    -s <SM model>                       [Optional] SM SSL connection or not, default no
    -h Help
e.g
    bash $0 -p 30300 -s -e ./mini-consensus
    bash $0 -p 30300 -s -c 10 -e ./mini-consensus
EOF

    exit 0
}

parse_params() {
    while getopts "c:d:e:l:p:sh" option; do
        case $option in
        c) node_count="$OPTARG" ;;
        d)
            output_dir="$OPTARG"
            mkdir -p "$output_dir"
            dir_must_exists "${output_dir}"
            ;;
        e) fisco_bcos_exec="$OPTARG" ;;
        l) listen_ip="$OPTARG" ;;
        p) listen_port="$OPTARG" ;;
        s) sm_mode="true" ;;
        h) help ;;
        *) help ;;
        esac
    done
}

print_result() {
    echo "=============================================================="
    LOG_INFO "listenIP          : ${listen_ip}"
    LOG_INFO "listenPort        : ${listen_port}"
    LOG_INFO "SSL Model         : ${ssl_model}"
    LOG_INFO "node count         : ${node_count}"
    LOG_INFO "output dir         : ${output_dir}"
    LOG_INFO "All completed. Files in ${output_dir}"
}

generate_all_node_scripts() {
    local output=${1}

    cat <<EOF >"${output}/start_all.sh"
#!/bin/bash
dirpath="\$(cd "\$(dirname "\$0")" && pwd)"
cd "\${dirpath}"

dirs=(\$(ls -l \${dirpath} | awk '/^d/ {print \$NF}'))
for dir in \${dirs[*]}
do
    if [[ -f "\${dirpath}/\${dir}/config.ini" && -f "\${dirpath}/\${dir}/start.sh" ]];then
        echo "try to start \${dir}"
        bash \${dirpath}/\${dir}/start.sh 
    fi
done
wait
EOF

    cat <<EOF >"${output}/stop_all.sh"
#!/bin/bash
dirpath="\$(cd "\$(dirname "\$0")" && pwd)"
cd "\${dirpath}"

dirs=(\$(ls -l \${dirpath} | awk '/^d/ {print \$NF}'))
for dir in \${dirs[*]}
do
    if [[ -f "\${dirpath}/\${dir}/config.ini" && -f "\${dirpath}/\${dir}/stop.sh" ]];then
        echo "try to stop \${dir}"
        bash \${dirpath}/\${dir}/stop.sh 
    fi
done
wait
EOF
}

generate_node_scripts() {
    local output=${1}

    cat <<EOF >"${output}/start.sh"
#!/bin/bash
dirpath="\$(cd "\$(dirname "\$0")" && pwd)"
cd "\${dirpath}"

config=\${dirpath}/config.ini
node=\$(basename \${dirpath})
pid=\$(ps aux | grep \${config} | grep -v grep | awk '{print \$2}')
if [ -n "\${pid}" ];then
        echo "\${node} is running, pid is \${pid}"
        exit 0
fi
nohup ../${binary_name} -c \${dirpath}/config.ini -g \${dirpath}/config.genesis &
EOF

    cat <<EOF >"${output}/stop.sh"
#!/bin/bash
dirpath="\$(cd "\$(dirname "\$0")" && pwd)"
cd "\${dirpath}"

config=\${dirpath}/config.ini
node=\$(basename \${dirpath})
pid=\$(ps aux | grep \${config} | grep -v grep | awk '{print \$2}')
if [ -z "\${pid}" ];then
        echo "\${node} is not running"
        exit 0
fi
kill -9 "\${pid}"
echo "stop \${node} successfully"
EOF

    cat <<EOF >"${output}/check.sh"
#!/bin/bash
dirpath="\$(cd "\$(dirname "\$0")" && pwd)"
cd "\${dirpath}"

config=\${dirpath}/config.ini
node=\$(basename \${dirpath})
pid=\$(ps aux | grep \${config} | grep -v grep | awk '{print \$2}')
if [ -z "\${pid}" ];then
        echo "\${node} is not running"
else
        echo "\${node} is running, pid=\${pid}"
fi
EOF
}

generate_connected_nodes() {
    local listen_ip="$1"
    local listen_port="$2"
    local count="$3"
    local p2p_host_list=""

    for ((i = 0; i < count; i++)); do
        mkdir -p "${output_dir}/node${i}"
        local delim=""
        if [[ $i == $((count - 1)) ]]; then
            delim=""
        else
            delim=","
        fi
        local port=$((listen_port + i))
        p2p_host_list="${p2p_host_list}${listen_ip}:${port}${delim}"
    done

    echo "${p2p_host_list}"
}

generate_node_cert() {
    local sm_mode="$1"
    local ca_cert_path="${2}"
    local node_cert_path="${3}"
    mkdir -p ${node_cert_path}
    if [[ "${sm_mode}" == "false" ]]; then
        gen_rsa_node_cert "${ca_cert_path}" "${node_cert_path}" "node" 2>&1
    else
        gen_sm_node_cert "${ca_cert_path}" "${node_cert_path}" "node" 2>&1
    fi
}

generate_chain_cert(){
    local sm_mode="$1"
    local chain_cert_path="$2"
    if [[ "${sm_mode}" == "false" ]]; then
        gen_chain_cert "${chain_cert_path}" 2>&1
    else
        gen_sm_chain_cert "${chain_cert_path}" 2>&1
    fi
}
generate_config_ini() {
    local output="${1}"
    local listen_port="${2}"
    local file_dir="./"
    cat <<EOF >"${output}"
[p2p]
    listen_ip=${listen_ip}
    listen_port=${listen_port}
    ; ssl or sm ssl
    sm_ssl=false
    nodes_path=${file_dir}
    nodes_file=${nodes_json_file_name}

[cert]
    ; directory the certificates located in
    ca_path=./conf
    ; the ca certificate file
    ca_cert=ca.crt
    ; the node private key file
    node_key=ssl.key
    ; the node certificate file
    node_cert=ssl.crt
EOF
    generate_common_ini "${output}"
}

generate_common_ini() {
    local output=${1}

    cat <<EOF >>"${output}"

[chain]
    ; use SM crypto or not, should nerver be changed
    sm_crypto=${sm_mode}
    ; the group id, should nerver be changed
    group_id=test_group
    ; the chain id, should nerver be changed
    chain_id=test_chain
    ; the block limit, should nerver be changed
    block_limit=1000

[security]
    private_key_path=conf/node.pem
    checkpoint_timeout=3000

[consensus]
    ; min block generation time(ms)
    min_seal_time=500

[executor]
    ; use the wasm virtual machine or not
    is_wasm=false

[storage]
    data_path=data

[txpool]
    limit=15000
    notify_worker_num=2
    verify_worker_num=2
[log]
    enable=true
    log_path=./log
    ; network statistics interval, unit is second, default is 60s
    stat_flush_interval=60
    ; info debug trace
    level=DEBUG
    ; MB
    max_log_file_size=200
EOF
}

generate_sm_config_ini() {
    local output=${1}
    local listen_port="${2}"
    cat <<EOF >"${output}"
[p2p]
    listen_ip=${listen_ip}
    listen_port=${listen_port}
    ; ssl or sm ssl
    sm_ssl=true
    nodes_path=${file_dir}
    nodes_file=${nodes_json_file_name}

[cert]
    ; directory the certificates located in
    ca_path=./
    ; the ca certificate file
    sm_ca_cert=sm_ca.crt
    ; the node private key file
    sm_node_key=sm_ssl.key
    ; the node certificate file
    sm_node_cert=sm_ssl.crt
    ; the node private key file
    sm_ennode_key=sm_enssl.key
    ; the node certificate file
    sm_ennode_cert=sm_enssl.crt
EOF
    generate_common_ini "${output}"
}

generate_nodes_json() {
    local output=${1}
    local p2p_host_list=""
    local ip_params="${2}"
    LOG_INFO "ip_params: ${ip_params}"
    local ip_array=(${ip_params//,/ })
    local ip_length=${#ip_array[@]}
    local i=0
    for ((; i < ip_length; i++)); do
        local ip=${ip_array[i]}
        local delim=""
        if [[ $i == $((ip_length - 1)) ]]; then
            delim=""
        else
            delim=","
        fi
        p2p_host_list="${p2p_host_list}\"${ip}\"${delim}"
    done

    cat <<EOF >"${output}"
{"nodes":[${p2p_host_list}]}
EOF
}

generate_config()
{
    local sm_mode="${1}"
    local node_config_path="${2}"
    local node_json_config_path="${3}"
    local connected_nodes="${4}"
    local listen_port="${5}"
    if [ "${sm_mode}" == "false" ]; then
        generate_config_ini "${node_config_path}" "${listen_port}"
    else
        generate_sm_config_ini "${node_config_path}" "${listen_port}"
    fi
    generate_nodes_json "${node_json_config_path}/${nodes_json_file_name}" "${connected_nodes}"
}

generate_node_account()
{
    local output_path="${1}"
    local node_index="${2}"
    if [ ! -d "${output_path}" ];then
        mkdir -p ${output_path}
    fi
    if [ ! -f /tmp/secp256k1.param ];then
        ${TASSL_CMD} ecparam -out /tmp/secp256k1.param -name secp256k1
    fi
    ${TASSL_CMD} genpkey -paramfile /tmp/secp256k1.param -out ${output_path}/node.pem
    # generate nodeid
    ${TASSL_CMD} ec -text -noout -in "${output_path}/node.pem" 2> /dev/null | sed -n '7,11p' | tr -d ": \n" | awk '{print substr($0,3);}' | cat >"$output_path"/node.nodeid
    local node_id=$(cat "${output_path}/node.nodeid")
    nodeid_list=$"${nodeid_list}node.${node_index}=${node_id}: 1
    "
}

generate_sm_node_account()
{
    local output_path="${1}"
    if [ ! -d "${output_path}" ];then
        mkdir -p ${output_path}
    fi
    if [ ! -f ${sm2_params} ];then
        generate_sm_sm2_param ${sm2_params}
    fi
    ${TASSL_CMD} genpkey -paramfile ${sm2_params} -out ${output_path}/node.pem 2>/dev/null
    $TASSL_CMD ec -in "$ndpath/sm_ssl.key" -text -noout 2>/dev/null | sed -n '7,11p' | sed 's/://g' | tr "\n" " " | sed 's/ //g' | awk '{print substr($0,3);}' | cat >"$output_path/sm_node.nodeid"
    local node_id=$(cat "${output_path}/sm_node.nodeid")
    nodeid_list=$"${nodeid_list}node.${node_index}=${node_id},1
    "
}

generate_genesis_config()
{
    local output=${1}
    local node_list=${2}

    cat <<EOF >"${output}"
[consensus]
    ; consensus algorithm now support PBFT(consensus_type=pbft)
    consensus_type=pbft
    ; the max number of transactions of a block
    block_tx_count_limit=1000
    ; in seconds, block consensus timeout, at least 3000ms
    consensus_timeout=3000
    ; the number of blocks generated by each leader
    leader_period=1
    ; the node id of consensusers
    ${node_list}

[tx]
    ; transaction gas limit
    gas_limit=300000000
EOF
}

main() {
    check_env
    check_and_install_tassl
    parse_params "$@"

    if [[ ! -f "$fisco_bcos_exec" ]]; then
        LOG_FALT "fisco bcos binary exec ${fisco_bcos_exec} not exist, please input the correct path."
    fi

    mkdir -p "${output_dir}"
    ca_cert_dir="${output_dir}"/ca

    cp "${fisco_bcos_exec}" "${output_dir}"
    connected_nodes=$(generate_connected_nodes 127.0.0.1 "${listen_port}" "${node_count}")

    generate_chain_cert "${sm_mode}" "${output_dir}/ca"
    # start_all.sh and stop_all.sh
    generate_all_node_scripts "${output_dir}"
    local i=0
    for ((i = 0; i < node_count; i++)); do
        account_dir=${output_dir}/node${i}/conf
        if [[ "${sm_mode}" == "false" ]]; then
            generate_node_account "${account_dir}" "${i}"
        else
            generate_sm_node_account "${account_dir}" "${i}"
        fi
    done
    for ((i = 0; i < node_count; i++)); do
        node_dir=${output_dir}/node${i}
        mkdir -p "${node_dir}"
        local port=$((listen_port + i))
        generate_node_cert "${sm_mode}" "${ca_cert_dir}" "${node_dir}/conf"
        # node config
        generate_config "${sm_mode}" "${node_dir}/config.ini" "${node_dir}" "${connected_nodes}" "${port}"
        generate_genesis_config "${node_dir}/config.genesis" "${nodeid_list}"
        generate_node_scripts "${node_dir}"
    done
    print_result
}

main "$@"