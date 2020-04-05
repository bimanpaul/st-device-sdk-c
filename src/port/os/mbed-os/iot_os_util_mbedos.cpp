/* ***************************************************************************
 *
 * Copyright 2019 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

#include <string.h>
#include "cmsis_os2.h"
//#include "event_groups.h"

#include "iot_error.h"
#include "iot_os_util.h"
#include "iot_debug.h"
#include "mbed.h"
#include "Queue2.h"
#include "us_ticker_api.h"
#include <stdio.h>

#define IOT_DEBUG(fmt,...)
#define IOT_INFO(fmt,...)

const unsigned int iot_os_max_delay = osWaitForever;
const unsigned int iot_os_true = 1;
const unsigned int iot_os_false = 0;

typedef void (*callbackFN)(void *);
/* Thread */
int iot_os_thread_create(void * thread_function, const char* name, int stack_size,
		void* data, int priority, iot_os_thread* thread_handle)
{
        Thread *thread = new Thread();
        //TODO: set thread attributes: priority, stack_size, name
        osStatus status = thread->start(callback((callbackFN)thread_function, data));
        if (status)
                return IOT_OS_FALSE;

        if (thread_handle)
                *thread_handle = thread;

        return IOT_OS_TRUE;
}

void iot_os_thread_delete(iot_os_thread thread_handle)
{
        Thread *thread = (Thread *)thread_handle;
        delete thread;
}

void iot_os_thread_yield()
{
        printf("CALL osThreadYield\n");
        osThreadYield();
        printf("END osThreadYield\n");
}


/* Queue */
iot_os_queue* iot_os_queue_create(int queue_length, int item_size)
{
        Queue2<void> *queue = new Queue2<void>(queue_length);
        IOT_DEBUG("Queue Create | Handle %p\n", queue);
        return queue;
}

int iot_os_queue_reset(iot_os_queue* queue_handle)
{
        //TODO: Not done yet
        printf("ERROR: iot_os_queue_reset\n");
	//return osMessageQueueReset(queue_handle);
	return true;
}

void iot_os_queue_delete(iot_os_queue* queue_handle)
{
	IOT_DEBUG("Queue Delete | Handle %p\n", queue_handle);
	Queue2<void> *queue = (Queue2<void> *)queue_handle;
	delete queue;
}

int iot_os_queue_send(iot_os_queue* queue_handle, void * data, unsigned int wait_time_ms)
{
	//TODO: set default priority
	IOT_DEBUG("BIMAN: iot_os_queue_send Queue Handle: %p \n", queue_handle);
	Queue2<void> *queue = (Queue2<void> *)queue_handle;
	if (queue->put (data) == osOK) {
		IOT_DEBUG("BIMAN: data %p put to queue \n", data);
		return IOT_OS_TRUE;
	}
	IOT_ERROR("Failed to put data %p in queue %p\n", data, queue_handle);
	return IOT_OS_FALSE;
}

int iot_os_queue_receive(iot_os_queue* queue_handle, void ** data, unsigned int wait_time_ms)
{
	//TODO: set default priority
	IOT_DEBUG("BIMAN: iot_os_queue_receive Queue Handle: %p \n", queue_handle);
	Queue2<void> *queue = (Queue2<void> *)queue_handle;

	if (queue->empty()) {
		IOT_ERROR("Queue Empty | Handle %p\n", queue);
		return IOT_OS_FALSE;
	}

	IOT_DEBUG("Queue Count: %d\n", queue->count());
	osEvent evt = queue->get();
	IOT_DEBUG("Queue STATUS: %d \n", evt.status);
	if (evt.status == osEventMessage) {
		*data = (void *)evt.value.p;
		IOT_DEBUG("Queue Return data %p\n", *data);
		return IOT_OS_TRUE;
	}
	IOT_ERROR("Failed to get data from queue %p\n", queue_handle);
	return IOT_OS_FALSE;
}

/* Event Group */
iot_os_eventgroup* iot_os_eventgroup_create(void)
{
        EventFlags *ef = new EventFlags();
        IOT_DEBUG("Create Event Group : %p", (void *)ef);
        return ef;
}

void iot_os_eventgroup_delete(iot_os_eventgroup* eventgroup_handle)
{
        EventFlags *ef = (EventFlags *)eventgroup_handle;
        IOT_DEBUG("Delete Event Group : %p", (void *)ef);
        delete ef;
}

unsigned int iot_os_eventgroup_wait_bits(iot_os_eventgroup* eventgroup_handle,
		const unsigned int bits_to_wait_for, const int clear_on_exit,
		const int wait_for_all_bits, const unsigned int wait_time_ms)
{
	//TODO: verify the logic of options
	EventFlags *ef = (EventFlags *)eventgroup_handle;
	uint32_t ret;

	if (wait_for_all_bits) {
		IOT_INFO("all_of_bits_to_wait_for: 0x%x | Handle: %p\n", bits_to_wait_for, eventgroup_handle);
		osDelay(200);
		ret =  ef->wait_all(bits_to_wait_for, wait_time_ms, clear_on_exit);
		osDelay(200);
		if (ret & osFlagsError) {
			IOT_ERROR("Did not receive Event for bits 0x%x | Handle: %p [0x%x]", bits_to_wait_for, eventgroup_handle, ret);
			return 0;
		}
		IOT_INFO("Received ALL | Handle: %p | Bits: 0x%x | Value: 0x%x\n", eventgroup_handle, bits_to_wait_for, ret);
		osDelay(200);
		return ret;
	}

	IOT_INFO("any_of_bits_to_wait_for: 0x%x | Handle: %p\n", bits_to_wait_for, eventgroup_handle);
	osDelay(200);
	ret = ef->wait_any(bits_to_wait_for, wait_time_ms, clear_on_exit);
	osDelay(200);
	if (ret & osFlagsError) {
		IOT_ERROR("Did not receive Event for bits 0x%x | Handle: %p [0x%x]", bits_to_wait_for, eventgroup_handle, ret);
		return 0;
	}
	IOT_INFO("iot_os_eventgroup_wait_bits: After wait_any | Handle: %p | Bits: 0x%x | Value: 0x%x\n", eventgroup_handle, bits_to_wait_for, ret);
	osDelay(200);
	return ret;
}

unsigned int iot_os_eventgroup_set_bits(iot_os_eventgroup* eventgroup_handle,
		const unsigned int bits_to_set)
{
        EventFlags *ef = (EventFlags *)eventgroup_handle;
        osDelay(300);
        IOT_INFO("bits_to_set: 0x%x | Handle: %p", bits_to_set, eventgroup_handle);
        osDelay(300);
        return ef->set(bits_to_set);
}

unsigned int iot_os_eventgroup_get_bits(iot_os_eventgroup* eventgroup_handle)
{
        EventFlags *ef = (EventFlags *)eventgroup_handle;
        return ef->get();
}

unsigned int iot_os_eventgroup_clear_bits(iot_os_eventgroup* eventgroup_handle,
		const unsigned int bits_to_clear)
{
        EventFlags *ef = (EventFlags *)eventgroup_handle;
        IOT_INFO("bits_to_set: 0x%x | Handle: %p\n", bits_to_clear, (void *)ef);
        return ef->clear(bits_to_clear);
}

/* Mutex */
int iot_os_mutex_init(iot_os_mutex* mutex)
{
	if (!mutex) {
		return IOT_ERROR_INVALID_ARGS;
	}
	Mutex *sem =  new Mutex();
	mutex->sem = sem;
	if (!mutex->sem) {
		return IOT_ERROR_MEM_ALLOC;
	}
	return IOT_ERROR_NONE;
}

int iot_os_mutex_lock(iot_os_mutex* mutex)
{
	if (!mutex) {
		return IOT_ERROR_INVALID_ARGS;
	}

	Mutex *sem = (Mutex *)mutex->sem;
	osStatus ret = sem->lock();
	if (ret == osOK)
		return IOT_ERROR_NONE;

	return  IOT_ERROR_BAD_REQ;

}

int iot_os_mutex_unlock(iot_os_mutex* mutex)
{
	if (!mutex) {
		return IOT_ERROR_INVALID_ARGS;
	}

        Mutex *sem = (Mutex *)mutex->sem;
        osStatus ret = sem->unlock();

        if (ret == osOK)
                return IOT_ERROR_NONE;

        return  IOT_ERROR_BAD_REQ;
}

void iot_os_mutex_destroy(iot_os_mutex* mutex)
{
	if (!mutex || !mutex->sem)
		return;

	delete (Mutex *)mutex->sem;
	mutex->sem = NULL;
}

/* Delay */
void iot_os_delay(unsigned int delay_ms)
{
        //TODO: multiply with appropriate value
        osDelay(delay_ms);
}

static unsigned int xTaskGetTickCount(void)
{
        return (us_ticker_read() / 1000L);
}

static void vTaskSetTimeOutState(unsigned int *mstime)
{
        *mstime = xTaskGetTickCount();
}

static bool xTaskCheckForTimeOut(unsigned int begin, unsigned int timeout)
{
        unsigned int mstime = xTaskGetTickCount();

        if (mstime < (begin + timeout))
                return false;

        return true;
}

//void iot_os_timer_count_ms(iot_os_timer timer, unsigned int timeout_ms)
//{
//        //TODO: MS to TICKS
//        osTimerStart(timer, timeout_ms);
//}
//
//unsigned int iot_os_timer_left_ms(iot_os_timer timer)
//{
//        return osTimerIsRunning(timer);
//}
//
//char iot_os_timer_isexpired(iot_os_timer timer)
//{
//        return (!osTimerIsRunning(timer));
//}
//
//int iot_os_timer_init(iot_os_timer *timer)
//{
//        //TODO: verify parameters
//        osTimerId_t tid;
//        osTimerAttr_t attr = {0 };
//        tid = osTimerNew(NULL, osTimerOnce, NULL, &attr);
//
//        *timer = tid;
//	return IOT_ERROR_NONE;
//}
//
//void iot_os_timer_destroy(iot_os_timer *timer)
//{
//	if (timer == NULL || *timer == NULL)
//		return;
//
//	osTimerDelete(*timer);
//	*timer = NULL;
//}

typedef struct Mbedos_Timer {
        unsigned int msWait;
        unsigned int beginTime;
} Mbedos_Timer;

void iot_os_timer_count_ms(iot_os_timer timer, unsigned int timeout_ms)
{
        ((Mbedos_Timer *)timer)->msWait = timeout_ms; /* convert milliseconds to ticks */
        vTaskSetTimeOutState(&((Mbedos_Timer *)timer)->beginTime); /* Record the time at which this function was entered. */
}

unsigned int iot_os_timer_left_ms(iot_os_timer timer)
{
        Mbedos_Timer *mbedos_timer = (Mbedos_Timer *)timer;
        unsigned int cTime = xTaskGetTickCount();

        if (cTime < (mbedos_timer->beginTime + mbedos_timer->msWait))
                return (mbedos_timer->beginTime + mbedos_timer->msWait - cTime);

        return 0;
}

char iot_os_timer_isexpired(iot_os_timer timer)
{
        Mbedos_Timer *mbedos_timer = (Mbedos_Timer *)timer;
        return xTaskCheckForTimeOut(mbedos_timer->beginTime,mbedos_timer->msWait);
}

int iot_os_timer_init(iot_os_timer *timer)
{
        *timer = malloc(sizeof(Mbedos_Timer));
        if (*timer == NULL)
                return IOT_ERROR_MEM_ALLOC;
        memset(*timer, '\0', sizeof(Mbedos_Timer));

        return IOT_ERROR_NONE;
}

void iot_os_timer_destroy(iot_os_timer *timer)
{
        if (timer == NULL || *timer == NULL)
                return;

        free(*timer);
        *timer = NULL;
}
