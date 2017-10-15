/*
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== Umsg.h ========
 *
 */

#ifndef Umsg__include
#define Umsg__include
#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */

#define Umsg_S_SUCCESS          (0)
#define Umsg_E_FAIL             (-1)
#define Umsg_E_MEMORY           (-2)
#define Umsg_E_ARG              (-3)
#define Umsg_E_PARAM            (-4)
#define Umsg_E_CONFIG           (-5)
#define Umsg_E_EMPTY            (-6)
#define Umsg_E_FULL             (-7)
#define Umsg_E_WAITINDEX        (-8)
#define Umsg_E_OSFAIL           (-9)

/*!
 *  @brief An instance handle type
 */
typedef struct Umsg_Object *Umsg_Handle;

/*!
 *  @brief  Structure defining params for creating an instance.
 */
typedef struct {
    UInt        msgSize;        /* size of each message (bytes)         */
    SizeT       poolCount;      /* number of messages in the pool       */
    Int         inboxCount;     /* max number of messages in the inbox  */
    UInt16      regionId;       /* shared region for instance object    */
} Umsg_Params;

/* =============================================================================
 *  APIs
 * =============================================================================
 */

/*!
 *  @brief      Allocate a message from the pool
 *
 *  @param      handle          Instance handle
 *
 *  @sa         Umsg_free
 */
Ptr Umsg_alloc(Umsg_Handle handle);

/*!
 *  @brief      Close a Umsg instance.
 *
 *  @param      handlePtr   Pointer to resource tracker instance handle
 *
 *  @sa         Umsg_open
 */
Void Umsg_close(Umsg_Handle *handlePtr);

/*!
 *  @brief      Create a Umsg instance
 *
 *  @param      params  Pointer to creation parameters. If NULL is passed,
 *                      default parameters are used.
 *
 *  @retval     Handle to the object
 *
 *  @sa         Umsg_delete
 */
Umsg_Handle Umsg_create(String name, Bool writer, UInt16 remoteProcId,
        Umsg_Params *params);

/*!
 *  @brief      Delete a Umsg instance.
 *
 *  @param      handlePtr   Pointer to resource tracker instance handle
 *
 *  @sa         Umsg_create
 */
Void Umsg_delete(Umsg_Handle *handlePtr);

/*!
 *  @brief      Runtime shutdown
 */
Void Umsg_destroy(Void);

/*!
 *  @brief      Return a message to the pool
 *
 *  @param      handle          Instance handle
 *
 *  @sa         Umsg_alloc
 */
Void Umsg_free(Umsg_Handle handle, Ptr msg);

/*!
 *  @brief      Receive a message from remote processor
 *
 *  @param      handle          Instance handle
 *
 *  @sa         Umsg_put
 */
Ptr Umsg_get(Umsg_Handle handle);

/*!
 *  @brief      Open an existing Umsg instance.
 *
 *  @param      name            Name of instance to open
 *  @param      handlePtr       Pointer to instance handle
 *
 *  @sa         Umsg_create
 */
Int Umsg_open(String name, Umsg_Handle *handlePtr);

/*!
 *  @brief      Send a message to remote processor
 *
 *  @param      handle          Instance handle
 *  @param      msg             Message to send
 *
 *  @sa         Umsg_get
 */
Int Umsg_put(Umsg_Handle handle, Ptr msg);

/*!
 *  @brief      Runtime setup
 */
Int Umsg_setup(Void);

/*!
 *  @brief      Initialize config-params structure with supplier-specified
 *              defaults before instance creation.
 *
 *  @param      params  Instance config-params structure.
 *
 *  @sa         Umsg_create
 */
Void Umsg_Params_init(Umsg_Params *params);

#if defined (__cplusplus)
}
#endif
#endif /* Umsg__include */
