// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef _JBIG2_CONTEXT_H_
#define _JBIG2_CONTEXT_H_

#include <list>
#include <utility>

#include "../../../../third_party/base/nonstd_unique_ptr.h"
#include "../../../include/fxcodec/fx_codec_def.h"
#include "JBig2_GeneralDecoder.h"
#include "JBig2_List.h"
#include "JBig2_Page.h"
#include "JBig2_Segment.h"

using CJBig2_CachePair = std::pair<const uint8_t*, CJBig2_SymbolDict*>;

#define JBIG2_SUCCESS 0
#define JBIG2_FAILED -1
#define JBIG2_ERROR_TOO_SHORT -2
#define JBIG2_ERROR_FATAL -3
#define JBIG2_END_OF_PAGE 2
#define JBIG2_END_OF_FILE 3
#define JBIG2_ERROR_FILE_FORMAT -4
#define JBIG2_ERROR_STREAM_TYPE -5
#define JBIG2_ERROR_LIMIT -6
#define JBIG2_FILE_STREAM 0
#define JBIG2_SQUENTIAL_STREAM 1
#define JBIG2_RANDOM_STREAM 2
#define JBIG2_EMBED_STREAM 3
#define JBIG2_MIN_SEGMENT_SIZE 11

class CJBig2_Context {
 public:
  static CJBig2_Context* CreateContext(
      const uint8_t* pGlobalData,
      FX_DWORD dwGlobalLength,
      const uint8_t* pData,
      FX_DWORD dwLength,
      int32_t nStreamType,
      std::list<CJBig2_CachePair>* pSymbolDictCache,
      IFX_Pause* pPause = NULL);

  static void DestroyContext(CJBig2_Context* pContext);

  int32_t getFirstPage(uint8_t* pBuf,
                       int32_t width,
                       int32_t height,
                       int32_t stride,
                       IFX_Pause* pPause);

  int32_t getFirstPage(CJBig2_Image** image, IFX_Pause* pPause);

  int32_t Continue(IFX_Pause* pPause);
  FXCODEC_STATUS GetProcessingStatus() { return m_ProcessingStatus; }

 private:
  enum JBig2State {
    JBIG2_OUT_OF_PAGE = 0,
    JBIG2_IN_PAGE,
  };

  CJBig2_Context(const uint8_t* pGlobalData,
                 FX_DWORD dwGlobalLength,
                 const uint8_t* pData,
                 FX_DWORD dwLength,
                 int32_t nStreamType,
                 std::list<CJBig2_CachePair>* pSymbolDictCache,
                 IFX_Pause* pPause);

  ~CJBig2_Context();

  int32_t decodeFile(IFX_Pause* pPause);

  int32_t decode_SquentialOrgnazation(IFX_Pause* pPause);

  int32_t decode_EmbedOrgnazation(IFX_Pause* pPause);

  int32_t decode_RandomOrgnazation_FirstPage(IFX_Pause* pPause);

  int32_t decode_RandomOrgnazation(IFX_Pause* pPause);

  CJBig2_Segment* findSegmentByNumber(FX_DWORD dwNumber);

  CJBig2_Segment* findReferredSegmentByTypeAndIndex(CJBig2_Segment* pSegment,
                                                    uint8_t cType,
                                                    int32_t nIndex);

  int32_t parseSegmentHeader(CJBig2_Segment* pSegment);

  int32_t parseSegmentData(CJBig2_Segment* pSegment, IFX_Pause* pPause);
  int32_t ProcessingParseSegmentData(CJBig2_Segment* pSegment,
                                     IFX_Pause* pPause);

  int32_t parseSymbolDict(CJBig2_Segment* pSegment, IFX_Pause* pPause);

  int32_t parseTextRegion(CJBig2_Segment* pSegment);

  int32_t parsePatternDict(CJBig2_Segment* pSegment, IFX_Pause* pPause);

  int32_t parseHalftoneRegion(CJBig2_Segment* pSegment, IFX_Pause* pPause);

  int32_t parseGenericRegion(CJBig2_Segment* pSegment, IFX_Pause* pPause);

  int32_t parseGenericRefinementRegion(CJBig2_Segment* pSegment);

  int32_t parseTable(CJBig2_Segment* pSegment);

  int32_t parseRegionInfo(JBig2RegionInfo* pRI);

  JBig2HuffmanCode* decodeSymbolIDHuffmanTable(CJBig2_BitStream* pStream,
                                               FX_DWORD SBNUMSYMS);

  void huffman_assign_code(int* CODES, int* PREFLEN, int NTEMP);

  void huffman_assign_code(JBig2HuffmanCode* SBSYMCODES, int NTEMP);

 private:
  CJBig2_Context* m_pGlobalContext;
  int32_t m_nStreamType;
  CJBig2_BitStream* m_pStream;
  int32_t m_nState;
  CJBig2_List<CJBig2_Segment> m_SegmentList;
  CJBig2_List<JBig2PageInfo> m_PageInfoList;
  CJBig2_Image* m_pPage;
  FX_BOOL m_bBufSpecified;
  size_t m_nSegmentDecoded;
  IFX_Pause* m_pPause;
  int32_t m_PauseStep;
  FXCODEC_STATUS m_ProcessingStatus;
  FX_BOOL m_bFirstPage;
  CJBig2_ArithDecoder* m_pArithDecoder;
  CJBig2_GRDProc* m_pGRD;
  JBig2ArithCtx* m_gbContext;
  nonstd::unique_ptr<CJBig2_Segment> m_pSegment;
  FX_DWORD m_dwOffset;
  JBig2RegionInfo m_ri;
  std::list<CJBig2_CachePair>* m_pSymbolDictCache;
};
#endif
