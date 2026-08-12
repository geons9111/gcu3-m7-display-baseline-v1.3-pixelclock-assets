/*
 * dpu_driver.c
 *
 * GCU3 M7 — MCUXpresso SDK fsl_dpu.h 上の DPU ドライバー。
 *
 * i.MX95 DPUは1つ以上のフレームバッファプレーンを合成し、その結果を内部DPIインターフェース経由でMIPI-DSIホストに供給します。
 * 本ドライバーは、ベースライン v1.1 §12 に従い、ロゴ / セーフディスプレイ / (将来の) ランタイム引き継ぎユースケースに十分な、単一のフルスクリーンプレーン（ブレンディングなし）を設定します（この段階では一般的なコンポジターではなく、DSIに供給するフェッチ/合成エンジンとしてのDPU）。
 */

/*
 * TODO [開発戦略 - Phase 1: NXP SDK直接活用 / Phase 2: Cdd_Dpuへの移行]:
 * 現在は NXP fsl_dpu.h を直接使用して迅速なHW Bring-upを行う。
 * HW動作検証完了後、本ファイルを Cdd_Dpu.c にリファクタリングし、AUTOSAR CDD規格インターフェースを実装する。
 */
#include "dpu_driver.h"

#include <string.h>
#include <stdint.h>

#include "fsl_dpu.h"

static DPU_Type *const s_dpu_base = DPU0; /* TBD: imx95lpd5evk19 のインスタンスを確認すること */

static dpu_display_timing_t s_timing;
static dpu_driver_config_t  s_cfg;

/* 静的ロゴ/セーフフレームバッファアドレス。
 * REQ-M7-04に従い、両方ともTCM/OCRAMに配置されます（ランタイムのファイルI/Oなし、デコードステップなし、ビルド時に組み込み）。
 * 実際のシンボルは生成されたフレームバッファアセットから提供されます（display_engine/framebuffer/ 参照、この変更ではまだ入力されていません）。
 */
extern const uint8_t g_gcu3_logo_framebuffer[];
extern const uint8_t g_gcu3_safe_framebuffer[];

int dpu_driver_init(const dpu_driver_config_t *config)
{
    if (config == NULL)
    {
        return -1;
    }
    s_cfg = *config;

    dpu_config_t dpu_init_config;
    DPU_GetDefaultConfig(&dpu_init_config);
    DPU_Init(s_dpu_base, &dpu_init_config);

    DPU_GetDefaultDisplayTimingConfig(&s_timing);
    s_timing.width  = s_cfg.width_px;
    s_timing.height = s_cfg.height_px;
    /* GCU3_DISPLAY_PIXEL_CLOCK_HZは現在0です（車両モニターのデータシート待ちのため、config/display_config.hで未定）。
     * ここで推測値を暗黙的に計算しないでください。pixel_clock_hzが未設定の場合は、呼び出し元 (display_engine) でクローズドフェイルにする必要があります。 */

    dpu_fetchunit_config_t fetch_config = {
        .width       = s_cfg.width_px,
        .height      = s_cfg.height_px,
        .format      = (dpu_pixel_format_t)s_cfg.pixel_format,
        .baseAddr    = s_cfg.framebuffer_addr,
        .stride      = (uint32_t)s_cfg.width_px * 2U, /* RGB565のデフォルト。
                                                           pixel_formatが変更された場合は再確認すること */
    };

    DPU_SetFetchUnitConfig(s_dpu_base, kDPU_FetchUnit0, &fetch_config);
    DPU_SetDisplayTimingConfig(s_dpu_base, kDPU_Display0, &s_timing);

    return 0;
}

int dpu_driver_start(void)
{
    DPU_EnableDisplay(s_dpu_base, kDPU_Display0, true);
    return 0;
}

void dpu_driver_stop(void)
{
    DPU_EnableDisplay(s_dpu_base, kDPU_Display0, false);
}

void dpu_driver_submit_frame(const char *frame_id)
{
    const uint8_t *fb = NULL;

    if (frame_id == NULL)
    {
        return;
    }
    if (strcmp(frame_id, "logo") == 0)
    {
        fb = g_gcu3_logo_framebuffer;
    }
    else if (strcmp(frame_id, "safe") == 0)
    {
        fb = g_gcu3_safe_framebuffer;
    }
    else
    {
        return; /* 不明なフレームID: クローズドフェイル、前のプレーンを維持 */
    }

    DPU_SetFetchUnitBaseAddr(s_dpu_base, kDPU_FetchUnit0, (uint32_t)(uintptr_t)fb);
    DPU_TriggerShadowLoad(s_dpu_base, kDPU_Display0);
}
