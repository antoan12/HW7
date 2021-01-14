#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include  "grades.h"
#include  "linked-list.h"

struct grades {
	struct list *students;
};

struct student {
    char *name;
	int id;
	int total;
	struct list *courses;
};

struct course {
	int grade;
	char *name;
};

// this is for visibilty of the code
typedef struct list List;
typedef struct grades Grades;
typedef struct student Student;
typedef struct course Course;
typedef enum {SUCCESS, FAIL} RESULT;

int clone_student (void* elem, void** out);
void destroy_student (void* elem);
struct student* student_list_search(struct list *students, int id);
struct course* course_list_search(struct list *courses, const char* name);
int clone_course (void* elem, void** out);
void destroy_course (void* elem);


struct grades* grades_init() {
	// allocate new memory for struct grades
	Grades *mygrades = (Grades*)malloc(sizeof(Grades));
	// check if the memory allocation failed
	if (!mygrades) {
		return NULL;
	}
	// init the ADT list of the students using the list_init from linked-list.h
	mygrades->students = list_init(clone_student,destroy_student);
	// check the return value of the list_init function
	if(!mygrades->students) {
		// free the memory of grades
		free(mygrades);
		return NULL;

	}
	return mygrades;
 }

void grades_destroy(struct grades *grades) {
	if (!grades) {
		return;
	}

	list_destroy(grades->students);
	free(grades);
}

int grades_add_student(struct grades *grades, const char *name, int id){
	if (!grades || !name) {
		return FAIL;
	}
	// check that the given id does not apper in the list of students
	if (student_list_search(grades->students, id)) {
		return FAIL;
	}
	struct student tmp;
	tmp.id = id;
	tmp.name = name;
	tmp.total = 0;
	tmp.courses = NULL;
	return list_push_back(grades->students, (void*)&tmp);
}

int grades_add_grade(struct grades *grades, const char *name, int id, int grade) { 
	if (grade < 0 || grade > 100 || !name || !grades) {
		return FAIL;
	}
	// check that the given id apper in the list of students 
	Student* tmp_std = student_list_search(grades->students, id);
	if (!tmp_std) {
		return FAIL;
	}

	// check if the course already exist in the student's courses list 
	if (course_list_search(tmp_std->courses,name)){
		return FAIL;
	}
	tmp_std->total+=grade;
	struct course tmp;
	tmp.grade = grade;
	tmp.name = name;

	return list_push_back(tmp_std->courses, (void*)&tmp);
}

float grades_calc_avg(struct grades *grades, int id, char **out){
	*out = NULL;
	if (!grades) {
		return -1;
	}

	Student *curr_stud = student_list_search(grades->students, id);
	
	// check that the given id does not apper in the list of students
	if (!curr_stud) {
		return -1;
	}

	float avg;

	if(!list_size(curr_stud->courses)){
		avg = 0;
	}
	else{
		avg = curr_stud->total/(float)list_size(curr_stud->courses);
	}

	char* tmp = (char*)malloc(sizeof(char)*(strlen(curr_stud->name) +1));
	if(!tmp){
		free(tmp);
		return FAIL;
	}
	strcpy(tmp, curr_stud->name);
	*out = tmp;

	return avg;
}

int grades_print_student(struct grades *grades, int id){
	if (!grades) {
		return FAIL;
	}
	Student *curr_stud = student_list_search(grades->students, id);
	// check that the given id does not apper in the list of students
	if (!curr_stud) {
		return FAIL;
	}
	printf("%s %d:",curr_stud->name,curr_stud->id);

	struct node *cursor_course;
	Course *it_course;
	cursor_course = list_begin(curr_stud->courses);
	
	for(;cursor_course;cursor_course = list_next(cursor_course)){
		it_course = (Course*)list_get(cursor_course);
		if (cursor_course != list_begin(curr_stud->courses)) {
			printf(",");
		}
		printf(" %s %d", it_course->name, it_course->grade);
	}
	printf("\n");
	return SUCCESS;
}

int grades_print_all(struct grades *grades){
	if (!grades) {
		return FAIL;
	}
	Student *it_stud; 
	struct node *cursor;
	cursor = list_begin(grades->students);
	for(;cursor;cursor = list_next(cursor)){
		it_stud = (Student*)list_get(cursor);
	    grades_print_student(grades,it_stud->id);
	}
	return SUCCESS;
}

/**
 * @brief Returns a pointer to the Struct student with "id" equals to id or NULL
 * in case the list don't contain a student with the given id
 * @param 
 */
struct student* student_list_search(struct list *students, int id){
	// iterator
	struct node *cursor;
	// set the iterator to the head of the list
	cursor = list_begin(students);
	void *elem;
	Student *lookup = NULL;

	// go over each node in the list
	for (;cursor; cursor = list_next(cursor)) {
		elem = list_get(cursor);
		lookup = (Student*)elem;
		// this is where the magic happenes !!!!!
		if (lookup->id == id) {
			return lookup;
		}
	}

	return NULL;
}

struct course* course_list_search(struct list *courses, const char* name){
	// iterator
	struct node *cursor;
	// set the iterator to the head of the list
	cursor = list_begin(courses);
	void *elem;
	Course *lookup = NULL;

	// go over each node in the list
	for (;cursor; cursor = list_next(cursor)) {
		elem = list_get(cursor);
		lookup = (Course*)elem;
		// this is where the magic happenes !!!!!
		if (!strcmp(lookup->name, name)) {
			return lookup;
		}
	}
	return NULL;
}

int clone_student (void* elem, void** out){
	if (!elem || !out) {
		return FAIL;
	}
	// cast the elem to struct student*
	Student *tmp = (Student*)elem;

	Student *copy = (Student*)malloc(sizeof(Student));
	if(!copy){
		return FAIL;
	}
	copy->name = (char*)malloc(sizeof(char)*(strlen(tmp->name) +1));
	if(!copy->name){
		free(copy);
		return FAIL;
	}
	copy->id = tmp->id;
	copy->total = tmp->total;
	strcpy(copy->name, tmp->name);
	copy->courses = list_init(clone_course, destroy_course);
	if(!copy->courses){
		free(copy->name);
		free(copy);
		return FAIL;
	}
	// cursor is the itrator to go over the old courses list 
	struct node *cursor;
	cursor = list_begin(tmp->courses);
	for(;cursor;cursor = list_next(cursor)){
		 list_push_back(copy->courses, list_get(cursor));
	}
	
	*out = (void*)copy;
	return SUCCESS;


}

int clone_course (void* elem, void** out){
	if (!elem || !out) {
		return FAIL;
	}
	// cast the elem to struct course*
	Course *tmp = (Course*)elem;

	Course *copy = (Course*)malloc(sizeof(Course));
	if(!copy){
		return FAIL;
	}
	copy->name = (char*)malloc(sizeof(char)*(strlen(tmp->name) +1));
	if(!copy->name){
		free(copy);
		return FAIL;
	}
	copy->grade = tmp->grade;
	strcpy(copy->name, tmp->name);

	*out = copy;
	return SUCCESS;
}

void destroy_student (void* elem){
	if(!elem){
		return;
	}
	// cast the elem to struct student*
	Student *tmp = (Student*)elem;
	free(tmp->name);
	list_destroy(tmp->courses);
	free(tmp);
}

void destroy_course (void* elem){
	if(!elem){
		return;
	}
	// cast the elem to struct student*
	Course *tmp = (Course*)elem;
	free(tmp->name);
	free(tmp);
}
