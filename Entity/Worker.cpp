#include "Worker.h"

XBT_LOG_EXTERNAL_DEFAULT_CATEGORY(test);

Worker::Worker() {
    task = nullptr;
    killed = false;
    skipSend = false;
    skipReceive = false;
    sendCount = 0;
    receivedCount = 0;
    lastSent = 0;
    doneSending = false;
    alive = true;
}

void Worker::operator()() {
    (*task)();
}

void Worker::setTask(std::shared_ptr<Task> t) {
    task = t;
}

std::string Worker::getHostName() {
    return task->getHostName();
}

bool Worker::isAlive() {
    return alive;
}

void Worker::makeDead() {
    alive = false;
    deadSince = simgrid::s4u::Engine::get_clock();
}

void Worker::makeAlive() {
    alive = true;
}

double Worker::getDeadSince() {
    return deadSince;
}

void Worker::messageSent() {
    ++sendCount;
    ++lastSent;
}

void Worker::messageReceived() {
    ++receivedCount;
}

int Worker::getSendCount() {
    return sendCount;
}
int Worker::getLastSent() {
    return lastSent;
}

int Worker::getReceivedCount() {
    return receivedCount;
}

void Worker::makeKilled() {
    killed = true;
}
void Worker::makeSkipReceive() {
    skipReceive = true;
}
void Worker::makeSkipSend() {
    skipSend = true;
}

bool Worker::isKilled() {
    return killed;
}
bool Worker::isSkipReceive() {
    return skipReceive;
}
bool Worker::isSkipSend() {
    return skipSend;
}
void Worker::done() {
    doneSending = true;
}
bool Worker::isDone() {
    return doneSending && alive;
}