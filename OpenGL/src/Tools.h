#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <mutex>
#include <iostream>
#include <list>
#include <map>
float generateHaltonNum(int index, int base);

std::vector<glm::vec2> generateHaltonSquence(int base1, int base2, int num);
#define Halton_Num 1024
static const std::vector<glm::vec2> HaltonSequence = generateHaltonSquence(2, 3, Halton_Num);

namespace Collisions
{
	//GJK
	glm::vec3 support(std::vector<glm::vec3>& vertices, glm::vec3 dir);
	float distancePoint2Triangle(glm::vec3 p, std::vector<glm::vec3>& vertices);
	float distancePoint2Triangle(glm::vec3 p, glm::vec3 pt, glm::vec3 normal);
	glm::vec3 normalPoint2Triangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);
	bool isInSimplex(std::vector<glm::vec3>& vertices);
	void updateSimplex(std::vector<glm::vec3>& simplex);
	bool GJK(std::vector<glm::vec3>& verticesA, std::vector<glm::vec3>& verticesB);

}
namespace myTools
{
	struct dialogBox
	{
		float minX;
		float maxX;
		float minY;
		float maxY;
		std::string words;
		dialogBox(float minx, float maxx, float miny, float maxy, std::string word) : minX(minx), maxX(maxx), minY(miny), maxY(maxy), words(word)
		{}
		dialogBox(const dialogBox& d)
		{
			minX = d.minX;
			maxX = d.maxX;
			minY = d.minY;
			maxX = d.maxY;
			words = d.words;
		}
		dialogBox()
		{
			minX = 450;
			maxX = 500;
			minY = 300;
			maxY = 400;
			words = "nothing";
		}
	};
	std::string clickDialogBox(dialogBox& box, float cursorX, float cursorY);
}

class eventSubscriber
{
private:

public:
	std::string data;
	void updateData(const std::string& d)
	{
		data = d;
		std::cout << data << std::endl;
	}
};
class eventPublisher
{
private:
	std::mutex publishing;
	std::mutex subscribing;
	int thread_id;
	std::list<std::string> eventList;//event list
	std::map<std::string, std::list<eventSubscriber>> subscribers;//subscriber map
public:
	void threadStart()
	{
		std::thread curThread(&eventPublisher::processEvent, this);
		curThread.detach();
	}
	void processEvent()
	{
		while (true)
		{
			//check published event
			publishing.lock();
			if (eventList.empty())
			{
				publishing.unlock();
				continue;
			}
			std::string curEvent = eventList.front();
			eventList.pop_front();
			publishing.unlock();
			//publish to subscribers
			subscribing.lock();
			auto iter = subscribers.find(curEvent);
			if (iter != subscribers.end())
			{
				for (auto it = iter->second.begin(); it != iter->second.end(); it++)
				{
					it->updateData(curEvent);
				}
			}
			subscribing.unlock();
		}
	}
	void publishEvent(std::string data)
	{
		publishing.lock();
		eventList.push_back(data);
		publishing.unlock();
	}
	void addSubscriber(eventSubscriber sb, std::string data)
	{
		subscribing.lock();
		subscribers[data].push_back(sb);
		subscribing.unlock();
	}
	//a example
	/*int main() {
		eventPublisher publisher;
		publisher.threadStart();
		eventSubscriber sb1;
		eventSubscriber sb2;
		publisher.addSubscriber(sb1, "w");
		publisher.addSubscriber(sb2, "z");
		publisher.publishEvent("w");
	}*/
};