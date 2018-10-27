#include <nlohmann/json.hpp>

#include <cstring>
#include <sstream>
#include <string>

// for convenience
using json = nlohmann::json;

#include "esp_log.h"

#include "wifi.h"

static const char *kTag = "Wifi Resource";

void WifiResource::HandleRequest(ICoapMessage const *request, ICoapMessage *response, CoapResult &result)
{
	//tcpip_adapter_ip_info_t ipinfo;
	// char payloadTemp[250], ip_address[16], ip_mask[16], ip_gateway[16];
	// char* pStrWorking = payloadTemp;

    CoapOption acceptOption;
    request->GetOption(acceptOption, CoapOptionValue::Accept, result);
    uint32_t accept = result == CoapResult::OK
        ? static_cast<CoapContentType>(AsUInt(acceptOption)->Value)
        : CoapContentType::TextPlain;

    ESP_LOGW(kTag, "request->GetOption result: %d. ContentType: %d", static_cast<int>(result), accept);

	// if( tcpip_adapter_get_ip_info( TCPIP_ADAPTER_IF_STA, &ipinfo ) != ESP_OK )
    // {
    //     ESP_LOGE( kTag, "tcpip_adapter_get_ip_info: failed" );
    //     return HANDLER_ERROR;
    // }

    if(accept == CoapContentType::TextPlain)
    {
        std::ostringstream output;
        output << "Ohai :3"; //"IP: "  << IP2STR( &ipinfo.ip )  << ", Mask: " << IP2STR( &ipinfo.netmask ) << ", Gateway: " << IP2STR( &ipinfo.gw );

        response->AddOption(CoapUIntOption(CoapOptionValue::ContentFormat, CoapContentType::TextPlain), result);
        response->SetPayload(output.str(), result);
        response->SetCode(CoapMessageCode::Content, result);
    }
    else if(accept == CoapContentType::ApplicationJson)
    {
        json output;
        output["message"] = "test";
    //     JSON_Value *root_value = json_value_init_object();
    //     JSON_Object *root_object = json_value_get_object(root_value);

    //     sprintf( (char*) pStrWorking, IPSTR, IP2STR( &ipinfo.ip ) );
    //     json_object_dotset_string(root_object, "wifi.ip", pStrWorking );
    //     sprintf( (char*) pStrWorking, IPSTR, IP2STR( &ipinfo.netmask ) );
    //     json_object_dotset_string(root_object, "wifi.mask", pStrWorking );
    //     sprintf( (char*) pStrWorking, IPSTR, IP2STR( &ipinfo.gw ) );
    //     json_object_dotset_string(root_object, "wifi.gateway", pStrWorking );

    //     json_serialize_to_buffer( root_value, payloadTemp, 250 );
    //     json_value_free(root_value);

    //     CoAP_AddCfOptionToMsg( pResp, COAP_CF_JSON );

        response->AddOption(CoapUIntOption(CoapOptionValue::ContentFormat, CoapContentType::ApplicationJson), result);
        response->SetPayload(output.dump(), result);
        response->SetCode(CoapMessageCode::Content, result);
    }
    else if(accept == CoapContentType::ApplicationCbor)
    {
        json output;
        output["message"] = "test";
    //     cn_cbor *map, *wifi;
    //     size_t enc_sz;

    //     wifi = cn_cbor_map_create(NULL);

    //     sprintf( (char*) ip_address, IPSTR, IP2STR( &ipinfo.ip ) );
    //     cn_cbor_map_put(wifi,
    //         cn_cbor_string_create("ip", NULL),
    //         cn_cbor_string_create(ip_address, NULL),
    //         NULL
    //     );
    //     sprintf( (char*) ip_mask, IPSTR, IP2STR( &ipinfo.netmask ) );
    //     cn_cbor_map_put(wifi,
    //         cn_cbor_string_create("mask", NULL),
    //         cn_cbor_string_create(ip_mask, NULL),
    //         NULL
    //     );

    //     sprintf( (char*) ip_gateway, IPSTR, IP2STR( &ipinfo.gw ) );
    //     cn_cbor_map_put(wifi,
    //         cn_cbor_string_create("gateway", NULL),
    //         cn_cbor_string_create(ip_gateway, NULL),
    //         NULL
    //     );

    //     map = cn_cbor_map_create(NULL);
    //     cn_cbor_map_put( map, cn_cbor_string_create("wifi", NULL), wifi, NULL );

    //     enc_sz = cn_cbor_encoder_write( (uint8_t*) payloadTemp, 0, 250, map);
    //     cn_cbor_free( map );

    //     CoAP_AddCfOptionToMsg( pResp, COAP_CF_CBOR );
    //     CoAP_SetPayload( pResp, (uint8_t*) payloadTemp, enc_sz, true );
    //     return HANDLER_OK;
        response->AddOption(CoapUIntOption(CoapOptionValue::ContentFormat, CoapContentType::ApplicationCbor), result);
        response->SetPayload(json::to_cbor(output), result);
        response->SetCode(CoapMessageCode::Content, result);
    }
    else
    {
        response->SetCode(CoapMessageCode::BadOption, result);
        result = CoapResult::Error;
    }

    // CoAP_SetPayload( pResp, (uint8_t*) payloadTemp, strlen( payloadTemp ), true );
}

WifiResource::WifiResource(ICoapInterface& coap)
    : _coap(coap)
{
    CoapResult result;
    this->_coap.CreateResource(this->_resource, this, "wifi", result);
    if (result != CoapResult::OK || this->_resource == nullptr)
    {
        ESP_LOGE( kTag, "CoAP_CreateResource returned NULL" );
        return;
    }

    this->_resource->RegisterHandler(CoapMessageCode::Get, result);
}
