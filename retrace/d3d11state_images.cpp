/**************************************************************************
 *
 * Copyright 2014 VMware, Inc.
 * Copyright 2011 Jose Fonseca
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/


#include <assert.h>

#include <iostream>
#include <algorithm>

#include "os.hpp"
#include "json.hpp"
#include "image.hpp"
#include "com_ptr.hpp"
#include "d3d11imports.hpp"
#include "d3d11state.hpp"
#include "d3d10state.hpp"
#include "dxgistate.hpp"


namespace d3dstate {


struct ResourceDesc
{
    D3D11_RESOURCE_DIMENSION Type;
    UINT Width;
    UINT Height;
    UINT Depth;
    UINT MipLevels;
    UINT ArraySize;
    DXGI_FORMAT Format;
    DXGI_SAMPLE_DESC SampleDesc;
    D3D11_USAGE Usage;
    UINT BindFlags;
    UINT CPUAccessFlags;
    UINT MiscFlags;
};


static void
getResourceDesc(ID3D11Resource *pResource, ResourceDesc *pDesc)
{
    pDesc->Type = D3D11_RESOURCE_DIMENSION_UNKNOWN;
    pDesc->Width = 0;
    pDesc->Height = 1;
    pDesc->Depth = 1;
    pDesc->MipLevels = 1;
    pDesc->ArraySize = 1;
    pDesc->Format = DXGI_FORMAT_UNKNOWN;
    pDesc->SampleDesc.Count = 1;
    pDesc->SampleDesc.Quality = 0;
    pDesc->Usage = D3D11_USAGE_DEFAULT;
    pDesc->BindFlags = 0;
    pDesc->CPUAccessFlags = 0;
    pDesc->MiscFlags = 0;

    pResource->GetType(&pDesc->Type);
    switch (pDesc->Type) {
    case D3D11_RESOURCE_DIMENSION_BUFFER:
        {
            D3D11_BUFFER_DESC Desc;
            static_cast<ID3D11Buffer *>(pResource)->GetDesc(&Desc);
            pDesc->Width = Desc.ByteWidth;
            pDesc->Usage = Desc.Usage;
            pDesc->BindFlags = Desc.BindFlags;
            pDesc->CPUAccessFlags = Desc.CPUAccessFlags;
            pDesc->MiscFlags = Desc.MiscFlags;
        }
        break;
    case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
        {
            D3D11_TEXTURE1D_DESC Desc;
            static_cast<ID3D11Texture1D *>(pResource)->GetDesc(&Desc);
            pDesc->Width = Desc.Width;
            pDesc->MipLevels = Desc.MipLevels;
            pDesc->ArraySize = Desc.ArraySize;
            pDesc->Format = Desc.Format;
            pDesc->Usage = Desc.Usage;
            pDesc->BindFlags = Desc.BindFlags;
            pDesc->CPUAccessFlags = Desc.CPUAccessFlags;
            pDesc->MiscFlags = Desc.MiscFlags;
        }
        break;
    case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
        {
            D3D11_TEXTURE2D_DESC Desc;
            static_cast<ID3D11Texture2D *>(pResource)->GetDesc(&Desc);
            pDesc->Width = Desc.Width;
            pDesc->Height = Desc.Height;
            pDesc->MipLevels = Desc.MipLevels;
            pDesc->ArraySize = Desc.ArraySize;
            pDesc->Format = Desc.Format;
            pDesc->SampleDesc = Desc.SampleDesc;
            pDesc->Usage = Desc.Usage;
            pDesc->BindFlags = Desc.BindFlags;
            pDesc->CPUAccessFlags = Desc.CPUAccessFlags;
            pDesc->MiscFlags = Desc.MiscFlags;
        }
        break;
    case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
        {
            D3D11_TEXTURE3D_DESC Desc;
            static_cast<ID3D11Texture3D *>(pResource)->GetDesc(&Desc);
            pDesc->Width = Desc.Width;
            pDesc->Height = Desc.Height;
            pDesc->Depth = Desc.Depth;
            pDesc->MipLevels = Desc.MipLevels;
            pDesc->Format = Desc.Format;
            pDesc->Usage = Desc.Usage;
            pDesc->BindFlags = Desc.BindFlags;
            pDesc->CPUAccessFlags = Desc.CPUAccessFlags;
            pDesc->MiscFlags = Desc.MiscFlags;
        }
        break;
    default:
        assert(0);
        break;
    }
}


static HRESULT
createResource(ID3D11Device *pDevice, const ResourceDesc *pDesc, ID3D11Resource **ppResource)
{
    switch (pDesc->Type) {
    case D3D11_RESOURCE_DIMENSION_BUFFER:
        {
            D3D11_BUFFER_DESC Desc;
            Desc.ByteWidth = pDesc->Width;
            Desc.Usage = pDesc->Usage;
            Desc.BindFlags = pDesc->BindFlags;
            Desc.CPUAccessFlags = pDesc->CPUAccessFlags;
            Desc.MiscFlags = pDesc->MiscFlags;
            return pDevice->CreateBuffer(&Desc, NULL, reinterpret_cast<ID3D11Buffer **>(ppResource));
        }
    case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
        {
            D3D11_TEXTURE1D_DESC Desc;
            Desc.Width = pDesc->Width;
            Desc.MipLevels = pDesc->MipLevels;
            Desc.ArraySize = pDesc->ArraySize;
            Desc.Format = pDesc->Format;
            Desc.Usage = pDesc->Usage;
            Desc.BindFlags = pDesc->BindFlags;
            Desc.CPUAccessFlags = pDesc->CPUAccessFlags;
            Desc.MiscFlags = pDesc->MiscFlags;
            return pDevice->CreateTexture1D(&Desc, NULL, reinterpret_cast<ID3D11Texture1D **>(ppResource));
        }
    case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
        {
            D3D11_TEXTURE2D_DESC Desc;
            Desc.Width = pDesc->Width;
            Desc.Height = pDesc->Height;
            Desc.MipLevels = pDesc->MipLevels;
            Desc.ArraySize = pDesc->ArraySize;
            Desc.Format = pDesc->Format;
            Desc.SampleDesc = pDesc->SampleDesc;
            Desc.Usage = pDesc->Usage;
            Desc.BindFlags = pDesc->BindFlags;
            Desc.CPUAccessFlags = pDesc->CPUAccessFlags;
            Desc.MiscFlags = pDesc->MiscFlags;
            return pDevice->CreateTexture2D(&Desc, NULL, reinterpret_cast<ID3D11Texture2D **>(ppResource));
        }
    case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
        {
            D3D11_TEXTURE3D_DESC Desc;
            Desc.Width = pDesc->Width;
            Desc.Height = pDesc->Height;
            Desc.Depth = pDesc->Depth;
            Desc.MipLevels = pDesc->MipLevels;
            Desc.Format = pDesc->Format;
            Desc.Usage = pDesc->Usage;
            Desc.BindFlags = pDesc->BindFlags;
            Desc.CPUAccessFlags = pDesc->CPUAccessFlags;
            Desc.MiscFlags = pDesc->MiscFlags;
            return pDevice->CreateTexture3D(&Desc, NULL, reinterpret_cast<ID3D11Texture3D **>(ppResource));
        }
    default:
        assert(0);
        *ppResource = NULL;
        return E_NOTIMPL;
    }
}


image::Image *
getSubResourceImage(ID3D11DeviceContext *pDeviceContext,
                    ID3D11Resource *pResource,
                    DXGI_FORMAT Format,
                    UINT ArraySlice,
                    UINT MipSlice)
{
    image::Image *image = NULL;
    UINT SubResource;
    D3D11_MAPPED_SUBRESOURCE MappedSubResource;
    HRESULT hr;

    if (!pResource) {
        return NULL;
    }

    com_ptr<ID3D11Device> pDevice;
    pDeviceContext->GetDevice(&pDevice);

    ResourceDesc Desc;
    getResourceDesc(pResource, &Desc);
    assert(ArraySlice < Desc.ArraySize);
    assert(MipSlice < Desc.MipLevels);
    assert(Desc.SampleDesc.Count > 0);

    SubResource = ArraySlice*Desc.MipLevels + MipSlice;

    /*
     * Resolve the subresource.
     */

    ResourceDesc ResolvedDesc = Desc;
    ResolvedDesc.Width  = std::max(Desc.Width  >> MipSlice, 1U);
    ResolvedDesc.Height = std::max(Desc.Height >> MipSlice, 1U);
    ResolvedDesc.Depth  = std::max(Desc.Depth  >> MipSlice, 1U);
    ResolvedDesc.ArraySize = 1;
    ResolvedDesc.MipLevels = 1;
    ResolvedDesc.SampleDesc.Count = 1;
    ResolvedDesc.SampleDesc.Quality = 0;
    ResolvedDesc.Usage = D3D11_USAGE_DEFAULT;
    ResolvedDesc.BindFlags = 0;
    ResolvedDesc.CPUAccessFlags = 0;
    ResolvedDesc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;

    com_ptr<ID3D11Resource> pResolvedResource;
    if (Desc.SampleDesc.Count == 1) {
        pResolvedResource = pResource;
    } else {
        hr = createResource(pDevice, &ResolvedDesc, &pResolvedResource);
        if (FAILED(hr)) {
            return NULL;
        }

        pDeviceContext->ResolveSubresource(pResolvedResource, 0, pResource, SubResource, Format);
        SubResource = 0;
    }

    /*
     * Stage the subresource.
     */

    ResourceDesc StagingDesc = ResolvedDesc;
    StagingDesc.Usage = D3D11_USAGE_STAGING;
    StagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    com_ptr<ID3D11Resource> pStagingResource;
    hr = createResource(pDevice, &StagingDesc, &pStagingResource);
    if (FAILED(hr)) {
        return NULL;
    }

    pDeviceContext->CopySubresourceRegion(pStagingResource, 0, 0, 0, 0, pResolvedResource, SubResource, NULL);

    /*
     * Map and read the subresource.
     */

    hr = pDeviceContext->Map(pStagingResource, 0, D3D11_MAP_READ, 0, &MappedSubResource);
    if (FAILED(hr)) {
        goto no_map;
    }

    image = ConvertImage(Format,
                         MappedSubResource.pData,
                         MappedSubResource.RowPitch,
                         StagingDesc.Width, StagingDesc.Height);

    pDeviceContext->Unmap(pStagingResource, 0);
no_map:
    return image;
}


static void
dumpShaderResourceViewImage(JSONWriter &json,
                            ID3D11DeviceContext *pDevice,
                            ID3D11ShaderResourceView *pShaderResourceView,
                            const char *shader,
                            UINT stage)
{
    if (!pShaderResourceView) {
        return;
    }

    com_ptr<ID3D11Resource> pResource;
    pShaderResourceView->GetResource(&pResource);
    assert(pResource);

    D3D11_SHADER_RESOURCE_VIEW_DESC Desc;
    pShaderResourceView->GetDesc(&Desc);

    UINT MipSlice = 0;
    UINT FirstArraySlice = 0;
    UINT ArraySize = 1;

    switch (Desc.ViewDimension) {
    case D3D11_SRV_DIMENSION_BUFFER:
    case D3D11_SRV_DIMENSION_BUFFEREX:
        break;
    case D3D11_SRV_DIMENSION_TEXTURE1D:
        MipSlice = Desc.Texture1D.MostDetailedMip;
        break;
    case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
        MipSlice = Desc.Texture1DArray.MostDetailedMip;
        FirstArraySlice = Desc.Texture1DArray.FirstArraySlice;
        ArraySize = Desc.Texture1DArray.ArraySize;
        break;
    case D3D11_SRV_DIMENSION_TEXTURE2D:
        MipSlice = Desc.Texture2D.MostDetailedMip;
        break;
    case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
        MipSlice = Desc.Texture2DArray.MostDetailedMip;
        FirstArraySlice = Desc.Texture2DArray.FirstArraySlice;
        ArraySize = Desc.Texture2DArray.ArraySize;
        break;
    case D3D11_SRV_DIMENSION_TEXTURE2DMS:
        break;
    case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
        FirstArraySlice = Desc.Texture2DMSArray.FirstArraySlice;
        ArraySize = Desc.Texture2DMSArray.ArraySize;
        break;
    case D3D11_SRV_DIMENSION_TEXTURE3D:
        MipSlice = Desc.Texture3D.MostDetailedMip;
        break;
    case D3D11_SRV_DIMENSION_TEXTURECUBE:
        MipSlice = Desc.TextureCube.MostDetailedMip;
        ArraySize = 6;
        break;
    case D3D11_SRV_DIMENSION_TEXTURECUBEARRAY:
        MipSlice = Desc.TextureCubeArray.MostDetailedMip;
        FirstArraySlice = Desc.TextureCubeArray.First2DArrayFace;
        ArraySize = 6 * Desc.TextureCubeArray.NumCubes;
        break;
    case D3D11_SRV_DIMENSION_UNKNOWN:
    default:
        assert(0);
        return;
    }

    for (UINT ArraySlice = FirstArraySlice; ArraySlice < FirstArraySlice + ArraySize; ++ArraySlice) {

        image::Image *image;
        image = getSubResourceImage(pDevice, pResource, Desc.Format, ArraySlice, MipSlice);
        if (image) {
            char label[64];
            _snprintf(label, sizeof label,
                      "%s_RESOURCE_%u_ARRAY_%u_LEVEL_%u",
                      shader, stage, ArraySlice, MipSlice);
            json.beginMember(label);
            json.writeImage(image, "UNKNOWN");
            json.endMember(); // *_RESOURCE_*
            delete image;
        }

    }
}


static image::Image *
getRenderTargetViewImage(ID3D11DeviceContext *pDevice,
                         ID3D11RenderTargetView *pRenderTargetView)
{
    if (!pRenderTargetView) {
        return NULL;
    }

    com_ptr<ID3D11Resource> pResource;
    pRenderTargetView->GetResource(&pResource);
    assert(pResource);

    D3D11_RENDER_TARGET_VIEW_DESC Desc;
    pRenderTargetView->GetDesc(&Desc);

    // TODO: Take the slice in consideration
    UINT MipSlice;
    switch (Desc.ViewDimension) {
    case D3D11_RTV_DIMENSION_BUFFER:
        MipSlice = 0;
        break;
    case D3D11_RTV_DIMENSION_TEXTURE1D:
        MipSlice = Desc.Texture1D.MipSlice;
        break;
    case D3D11_RTV_DIMENSION_TEXTURE1DARRAY:
        MipSlice = Desc.Texture1DArray.MipSlice;
        break;
    case D3D11_RTV_DIMENSION_TEXTURE2D:
        MipSlice = Desc.Texture2D.MipSlice;
        break;
    case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:
        MipSlice = Desc.Texture2DArray.MipSlice;
        break;
    case D3D11_RTV_DIMENSION_TEXTURE2DMS:
        MipSlice = 0;
        break;
    case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
        MipSlice = 0;
        break;
    case D3D11_RTV_DIMENSION_TEXTURE3D:
        MipSlice = Desc.Texture3D.MipSlice;
        break;
    case D3D11_RTV_DIMENSION_UNKNOWN:
    default:
        assert(0);
        return NULL;
    }

    return getSubResourceImage(pDevice, pResource, Desc.Format, 0, MipSlice);
}


static image::Image *
getDepthStencilViewImage(ID3D11DeviceContext *pDevice,
                         ID3D11DepthStencilView *pDepthStencilView)
{
    if (!pDepthStencilView) {
        return NULL;
    }

    com_ptr<ID3D11Resource> pResource;
    pDepthStencilView->GetResource(&pResource);
    assert(pResource);

    D3D11_DEPTH_STENCIL_VIEW_DESC Desc;
    pDepthStencilView->GetDesc(&Desc);

    // TODO: Take the slice in consideration
    UINT MipSlice;
    switch (Desc.ViewDimension) {
    case D3D11_DSV_DIMENSION_TEXTURE1D:
        MipSlice = Desc.Texture1D.MipSlice;
        break;
    case D3D11_DSV_DIMENSION_TEXTURE1DARRAY:
        MipSlice = Desc.Texture1DArray.MipSlice;
        break;
    case D3D11_DSV_DIMENSION_TEXTURE2D:
        MipSlice = Desc.Texture2D.MipSlice;
        break;
    case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
        MipSlice = Desc.Texture2DArray.MipSlice;
        break;
    case D3D11_DSV_DIMENSION_TEXTURE2DMS:
        MipSlice = 0;
        break;
    case D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY:
        MipSlice = 0;
        break;
    case D3D11_DSV_DIMENSION_UNKNOWN:
    default:
        assert(0);
        return NULL;
    }

    return getSubResourceImage(pDevice, pResource, Desc.Format, 0, MipSlice);
}


static void
dumpStageTextures(JSONWriter &json, ID3D11DeviceContext *pDevice, const char *stageName,
                  UINT NumViews,
                  ID3D11ShaderResourceView **ppShaderResourceViews)
{
    for (UINT i = 0; i < NumViews; ++i) {
        if (!ppShaderResourceViews[i]) {
            continue;
        }

        dumpShaderResourceViewImage(json, pDevice, ppShaderResourceViews[i], stageName, i);

        ppShaderResourceViews[i]->Release();
    }
}


void
dumpTextures(JSONWriter &json, ID3D11DeviceContext *pDevice)
{
    json.beginMember("textures");
    json.beginObject();

    ID3D11ShaderResourceView *pShaderResourceViews[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT];

    pDevice->PSGetShaderResources(0, ARRAYSIZE(pShaderResourceViews), pShaderResourceViews);
    dumpStageTextures(json, pDevice, "PS", ARRAYSIZE(pShaderResourceViews), pShaderResourceViews);

    pDevice->VSGetShaderResources(0, ARRAYSIZE(pShaderResourceViews), pShaderResourceViews);
    dumpStageTextures(json, pDevice, "VS", ARRAYSIZE(pShaderResourceViews), pShaderResourceViews);

    pDevice->GSGetShaderResources(0, ARRAYSIZE(pShaderResourceViews), pShaderResourceViews);
    dumpStageTextures(json, pDevice, "GS", ARRAYSIZE(pShaderResourceViews), pShaderResourceViews);

    json.endObject();
    json.endMember(); // textures
}


image::Image *
getRenderTargetImage(ID3D11DeviceContext *pDevice) {
    com_ptr<ID3D11RenderTargetView> pRenderTargetView;
    pDevice->OMGetRenderTargets(1, &pRenderTargetView, NULL);

    image::Image *image = NULL;
    if (pRenderTargetView) {
        image = getRenderTargetViewImage(pDevice, pRenderTargetView);
    }

    return image;
}


void
dumpFramebuffer(JSONWriter &json, ID3D11DeviceContext *pDevice)
{
    json.beginMember("framebuffer");
    json.beginObject();

    ID3D11RenderTargetView *pRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
    ID3D11DepthStencilView *pDepthStencilView;
    pDevice->OMGetRenderTargets(ARRAYSIZE(pRenderTargetViews), pRenderTargetViews,
                                &pDepthStencilView);

    for (UINT i = 0; i < ARRAYSIZE(pRenderTargetViews); ++i) {
        if (!pRenderTargetViews[i]) {
            continue;
        }

        image::Image *image;
        image = getRenderTargetViewImage(pDevice, pRenderTargetViews[i]);
        if (image) {
            char label[64];
            _snprintf(label, sizeof label, "RENDER_TARGET_%u", i);
            json.beginMember(label);
            json.writeImage(image, "UNKNOWN");
            json.endMember(); // RENDER_TARGET_*
            delete image;
        }

        pRenderTargetViews[i]->Release();
    }

    if (pDepthStencilView) {
        image::Image *image;
        image = getDepthStencilViewImage(pDevice, pDepthStencilView);
        if (image) {
            json.beginMember("DEPTH_STENCIL");
            json.writeImage(image, "UNKNOWN");
            json.endMember();
            delete image;
        }

        pDepthStencilView->Release();
    }

    json.endObject();
    json.endMember(); // framebuffer
}


} /* namespace d3dstate */
