#pragma once

#ifndef _FIELD_DEFINATION_
#define _FIELD_DEFINATION_

#include <string>
#include <memory>


namespace Reporter
{
	class BasicField
	{
	protected:
		std::string name;

	public:
		BasicField(const std::string& name_) : name(name_) {}
		BasicField() {}

		BasicField(const BasicField& field) : name(field.name) {}
		BasicField(BasicField&& field) : name( std::move(field.name) ) {}

		BasicField& operator = (const BasicField& field) { name = field.name; return *this; }
		BasicField& operator = (BasicField&& field) { name = std::move(field.name); return *this; }
		
		virtual std::string getValue() = 0;
		std::string getName() { return name; }
	};

	template< typename T >
	class Field : public BasicField
	{
		T value;
	public:
		Field(const std::string& name, const T& value_) : BasicField(name), value(value_) {}
		Field(const std::string& name) : BasicField(name) {}
		Field() :BasicField(), value() {}

		Field(const Field& field) : BasicField(field.name), value(field.value) {}
		Field(Field&& field) : BasicField(std::move(field.name)), value(std::move(field.value)) {}

		Field& operator = (const Field& field) { name = field.name; value = field.value; return *this; }
		Field& operator = (Field&& field) { name = std::move(field.name); value = std::move(field.value); return *this; }
		
		void getValue(T& value_) { value_ = value; }
		std::pair<std::string, T> getDetails() { return std::make_pair(name, value); }
		std::string getValue() override { return std::to_string(value); }

		void setValue(const T& value_) { value = value_; }
	};

	template<>
	class Field<std::string> : public BasicField 
	{
		std::string value;
	public:
		Field(const std::string& name, const std::string& value_) : BasicField(name), value(value_) {}
		Field(const std::string& name) : BasicField(name) {}
		Field(const Field& field) : BasicField(field.name), value(field.value) {}

		std::pair<std::string, std::string> getDetails() { return std::make_pair(name, value); }
		std::string getValue() override { return value; }

		void setValue(const std::string& value_) { value = value_; }
	};

	struct FieldWrapper
	{
		std::shared_ptr<BasicField> field;

		bool operator < (const FieldWrapper& fldwrap) const
		{
			return field->getName() < fldwrap.field->getName();
		}

		template<typename T>
		FieldWrapper(const std::string & name_, const T& t) : field(new Field<T>(name_, t)) {}

		FieldWrapper(const FieldWrapper& wrapper) : field(wrapper.field) {}
		FieldWrapper(FieldWrapper&& wrapper) : field( std::move(wrapper.field) ) {}
		FieldWrapper() : field(nullptr) {}

		FieldWrapper& operator = (const FieldWrapper& wrapper) { field = wrapper.field; return *this; }
		FieldWrapper& operator = (FieldWrapper&& wrapper) { field = std::move(wrapper.field); return *this; }
	};
}

#endif