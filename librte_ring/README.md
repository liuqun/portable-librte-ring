# DPDK 环形队列 API 函数

代码提取自:
1. 开源项目 https://github.com/opencoff/portable-lib ，作者: Sudhi Herle
2. DPDK 开源项目：[librte_ring库的头文件 `rte_ring.h` ](https://github.com/DPDK/dpdk/blob/master/lib/librte_ring/rte_ring.h) - 版权归属: 英特尔公司
3. FreeBSD 操作系统源码：FreeBSD 头文件 `bufring.h` - 作者: Kip Macy


# 数据结构及 API 函数

## 公开数据结构 `struct rte_ring`
```
struct __CACHELINE_ALIGNED rte_ring {
    struct __CACHELINE_ALIGNED {
        uint32_t size;           /**< Size of the ring. */
        uint32_t mask;           /**< Mask (size-1) of ring. */
        uint16_t sp_enq;         /**< True if single producer */
        uint16_t sc_deq;         /**< True if single consumer */
    };

    /** Ring producer & consumer status. */
    struct __CACHELINE_ALIGNED __rte_index prod;
    struct __CACHELINE_ALIGNED __rte_index cons;

    void * volatile __CACHELINE_ALIGNED ring[0];
};
```

## 私有数据结构 `struct __rte_index`
```
struct __rte_index {
    atomic_uint_fast32_t head;
    atomic_uint_fast32_t tail;
};
```


## API 函数 `rte_ring_create()`
```
/**
 * Create a new ring named *name* in memory.
 *
 * This function uses ``memzone_reserve()`` to allocate memory. Its size is
 * set to *count*, which must be a power of two. Water marking is
 * disabled by default.
 * Note that the real usable ring size is *count-1* instead of
 * *count*.
 *
 * @param count
 *   The size of the ring (must be a power of 2).
 * @param flags
 *   An OR of the following:
 *    - RING_F_SP_ENQ: If this flag is set, the default behavior when
 *      using ``rte_ring_enqueue()`` or ``rte_ring_enqueue_bulk()``
 *      is "single-producer". Otherwise, it is "multi-producers".
 *    - RING_F_SC_DEQ: If this flag is set, the default behavior when
 *      using ``rte_ring_dequeue()`` or ``rte_ring_dequeue_bulk()``
 *      is "single-consumer". Otherwise, it is "multi-consumers".
 * @return
 *   On success, the pointer to the new allocated ring. NULL on error with
 */

static inline
struct rte_ring *rte_ring_create(unsigned count, unsigned flags);
```

### 注意:

1. 函数`rte_ring_create()`的参数经过简化只保留了count和flags两项；
2. DPDK原始版本要求设置4个参数，我们并不需要多出来的name和id；


## API 函数 `rte_ring_destory()`

销毁函数的参数个数只有一个，与DPDK原始版本一致：
```
static inline void rte_ring_destroy(struct rte_ring *r);
```


# C语言头文件列表

- DPDK环形队列库 ------ `#include "rte_ring.h"`
- 自定义错误处理函数 -- `#include "error.h"`
- 自定义CPU检测函数 --- `#include "utils/cpu.h"`
- 其他自定义工具函数 -- `#include "utils/utils.h"`


# 测试程序
主程序 `test_rte_ringbuf.c` 用于测试上述提到过的 API 函数 `rte_ring_XXX()`

