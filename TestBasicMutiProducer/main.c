#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <pthread.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <semaphore.h>
#include "cpu.h"   // 用到的函数: sys_cpu_getavail() 等
#include "utils/utils.h" // 用到若干自定义函数: timenow() 和 sys_cpu_timestamp() 等
#include "fast/vect.h"   // 用到的宏: VECT_TYPEDEF()
#include "error.h"       // 用到若干自定义错误处理函数

#include "rte_ring.h"

#define PRINTF(...) fprintf(stdout, __VA_ARGS__)
#undef PRINTF
#define PRINTF(...) do {;} while(0) // 禁用PRINTF()宏函数


static void
basic_sp_test()
{
    int s;
    struct rte_ring *q;
    union {
        void *vp;
        uint64_t v;
    } u;


    const unsigned int count = 4;
    const unsigned int flags = 0;// 参数 flags 默认值等于 0: 表示支持 MPMC “多生产者+多消费者”
                                //#define RING_F_SP_ENQ 0x0001 /**< The default enqueue is "single-producer". */
                                //#define RING_F_SC_DEQ 0x0002 /**< The default dequeue is "single-consumer". */
                                //
                                // RING_F_SP_ENQ: If this flag is set, the default behavior when
                                //      using rte_ring_enqueue() or rte_ring_enqueue_bulk()
                                //      is "single-producer". Otherwise, it is "multi-producers".
                                // RING_F_SC_DEQ: If this flag is set, the default behavior when
                                //      using rte_ring_dequeue() or rte_ring_dequeue_bulk()

    PRINTF("调用 API 函数 rte_ring_create(%d, %d):\n", (int)count, (int)flags);
    PRINTF("  参数 count=%d : 固定最大容量为 count-1=%d\n", (int)count, (int)count-1);
    q = rte_ring_create(count, flags);

    PRINTF("调用 API 函数 rte_ring_sp_enqueue() 插入若干测试节点\n");
    s = rte_ring_sp_enqueue(q, (void *)10); assert(s == 1);
    s = rte_ring_sp_enqueue(q, (void *)11); assert(s == 1);
    s = rte_ring_sp_enqueue(q, (void *)12); assert(s == 1);

    assert(rte_ring_count(q) == 3);
    PRINTF("调用 API 函数 rte_ring_count() 返回值为 %d\n", (int)rte_ring_count(q));

    s = rte_ring_sp_enqueue(q, (void *)13); assert(s < 0);
    assert(rte_ring_full(q));
    assert(!rte_ring_empty(q));

    s = rte_ring_mc_dequeue(q, &u.vp); assert(s == 1);
    assert(u.v == 10);

    s = rte_ring_mc_dequeue(q, &u.vp); assert(s == 1);
    assert(u.v == 11);

    s = rte_ring_mc_dequeue(q, &u.vp); assert(s == 1);
    assert(u.v == 12);

    assert(rte_ring_count(q) == 0);
    s = rte_ring_mc_dequeue(q, &u.vp); assert(s < 0);

    assert(rte_ring_empty(q));
    assert(!rte_ring_full(q));
    PRINTF("调用 API 函数 rte_ring_destroy() 释放之前申请的资源\n");
    rte_ring_destroy(q);
}

static void
basic_mp_test()
{
    int s;
    struct rte_ring *q;
    union {
        void *vp;
        uint64_t v;
    } u;


    q = rte_ring_create(4, 0);

    s = rte_ring_mp_enqueue(q, (void *)10); assert(s == 1);
    s = rte_ring_mp_enqueue(q, (void *)11); assert(s == 1);
    s = rte_ring_mp_enqueue(q, (void *)12); assert(s == 1);

    assert(rte_ring_count(q) == 3);

    s = rte_ring_mp_enqueue(q, (void *)13); assert(s < 0);
    assert(rte_ring_full(q));
    assert(!rte_ring_empty(q));

    s = rte_ring_mc_dequeue(q, &u.vp); assert(s == 1);
    assert(u.v == 10);

    s = rte_ring_mc_dequeue(q, &u.vp); assert(s == 1);
    assert(u.v == 11);

    s = rte_ring_mc_dequeue(q, &u.vp); assert(s == 1);
    assert(u.v == 12);

    assert(rte_ring_count(q) == 0);
    s = rte_ring_mc_dequeue(q, &u.vp); assert(s < 0);

    assert(rte_ring_empty(q));
    assert(!rte_ring_full(q));

    rte_ring_destroy(q);
}

int main(int argc, char** argv)
{
    int ncpu = sys_cpu_getavail(),
        half = ncpu >> 1;
    int p = 0,
        c = 0;


    if (half == 0)
        half = 1;

    program_name = argv[0];

    if (argc > 2) {
        int z = atoi(argv[2]);
        if (z < 0)
            error(0, 0, "Ignoring invalid consumer count '%d'; defaulting to %d", z, half);
        else if (z > 0)
            c = z;
    }

    if (argc > 1) {
        int z = atoi(argv[1]);
        if (z < 0)
            error(0, 0, "Ignoring invalid producer count '%d'; defaulting to %d", z, half);
        else if (z > 0)
            p = z;
    }

    if (c == 0)
        c = half;
    if (p == 0)
        p = half;

    printf("【测试1】API基本测试：检查“单生产者-多消费者”API函数执行结果是否符合预期\n"
           "待测API函数如下:\n"
           "  1. rte_ring_create()\n"
           "  2. rte_ring_sp_enqueue()\n"
           "  3. rte_ring_mc_dequeue()\n"
           "  4. rte_ring_count()\n"
           "  5. rte_ring_full()\n"
           "  6. rte_ring_empty()\n"
           "  7. rte_ring_destory()\n"
           "开始执行测试用例 basic_sp_test() ...\n");
    basic_sp_test();
    printf("API基本测试1结束，一切正常，回到main()函数等待进行下一个测试...\n\n");

    printf("【测试2】API基本测试：检查“多生产者-多消费者”API函数执行结果是否符合预期\n"
           "待测API函数为:\n"
           "  8. rte_ring_mp_enqueue()\n"
           "开始执行测试用例 basic_mp_test() ...\n");
    basic_mp_test();
    printf("API基本测试2结束，一切正常，回到main()函数等待进行下一个测试...\n\n");

    return 0;
}
